#include <cassert>
#include <cmath>
#include <ranges>
#include <algorithm>
#include <numbers>
#include <iostream>
#include <fstream>
#include <vector>
#include <complex>
#include <format>
#include <numeric>
#include <AudioFFT.h>

class FuriesTransform {
public:
    static double KaiserParamBeta(double lobe_level) {
        assert(lobe_level >= 0);
        if (lobe_level < 21.0) {
            return 0.0;
        }
        else if (lobe_level <= 50.0) {
            return 0.5842 * std::pow(lobe_level - 21.0, 0.4)
                + 0.07886 * (lobe_level - 21.0);
        }
        else {
            return 0.1102 * (lobe_level - 8.7);
        }
    }

    int KaiserWindow(float side_lobe_level,
                     float main_lobe_width) {
        auto beta = KaiserParamBeta(side_lobe_level);
        auto alpha = beta / std::numbers::pi;
        auto main_width_in_bin = 2.0 * std::sqrt(1.0 + alpha * alpha);
        auto n = static_cast<int>(std::ceil(main_width_in_bin / main_lobe_width));
        if (n % 2 == 0) ++n;

        window_.resize(n);
        dwindow_.resize(n);

        auto inc = 2.0 / (n - 1);
        auto down = 1.0f / std::cyl_bessel_i(0, beta);
        for (int i = 0; i < n; ++i) {
            auto t = -1.0 + i * inc;
            auto arg = std::sqrt(1.0 - t * t);
            window_[i] = std::cyl_bessel_i(0, beta * arg) * down;

            constexpr auto kTimeDelta = 0.001f;
            if (i == 0) {
                dwindow_.front() = (std::cyl_bessel_i(0, beta * std::sqrt(1.0 - (t + kTimeDelta) * (t + kTimeDelta))) * down - window_.front()) / kTimeDelta;
            }
            else if (i == n - 1) {
                dwindow_.back() = (std::cyl_bessel_i(0, beta * std::sqrt(1.0 - (t - kTimeDelta) * (t - kTimeDelta))) * down - window_.back()) / -kTimeDelta;
            }
            else {
                dwindow_[i] = std::cyl_bessel_i(1, beta * arg) * beta * (-t / arg) * down;
            }
        }

        auto window_sum = std::accumulate(window_.cbegin(), window_.cend(), 0.0f);
        window_scale_ = 2.0f / window_sum;
        dwindow_scale_ = window_scale_ / (2 * std::numbers::pi);

        return n;
    }

    int TaylorWindow(double side_lobe_level, double norm_freq, int nb) {
        auto wc = std::numbers::sqrt2 * norm_freq * std::numbers::pi;
        auto r = std::pow(10.0, -side_lobe_level / 20.0);
        auto Mf = 1 + std::acosh(1.0 / r) / std::acosh(1.0 / std::cos(wc / 2));
        auto window_size = static_cast<int>(std::ceil(Mf)) + 1;
        if ((window_size & 1) == 0)
            ++window_size;
        // auto R = std::pow(10.0, side_lobe_level / 20.0);
        // std::vector<double> n;
        // n.resize(nb - 1);
        // std::iota(n.begin(), n.end(), 1);
        // auto A = std::acosh(R) / std::numbers::pi_v<double>;
        // auto xn_ranges = n | std::views::transform([A](auto nx){
        //                                                         return nx / std::sqrt(A * A + (nx - 0.5) * (nx - 0.5));
        //                                                     })
        //                                                     | std::views::transform([A](auto sigma){
        //                                                         return sigma * std::sqrt(A * A + (sigma - 0.5) * (sigma - 0.5));
        //                                                     });
        // auto h = n;
        // auto n2 = n | std::views::transform([](auto x) -> decltype(x) { return x * x; });
        // auto xn2 = xn_ranges | std::views::transform([](auto x) -> decltype(x) { return 1 / (x * x); });
        // auto prod = std::inner_product(n2.begin(), n2.end(), xn2.begin(), 0.0);
        // std::ranges::transform(n, xn_ranges, h.begin(), [prod, nb](auto nx, auto xnx) -> decltype(xnx) {
        //     auto a = std::tgamma(nb);
        //     auto b = std::tgamma(nx + nb);
        //     auto c = std::tgamma(nb - nx);
        //     return a * a / (b * c * prod);
        // });

        // auto fs_view = std::views::iota(0, window_size) | std::views::transform([window_size](int x) {
        //                                                         return x / (window_size - 1.0) - 0.5;
        //                                                     });
        // std::vector<double> fs(fs_view.begin(), fs_view.end());
        // window_.resize(window_size);
        // dwindow_.resize(window_size);
        // auto a = h | std::views::transform([](auto x) { return x * 2; });

        // constexpr auto kTimeDelta = 0.001;
        // for (int i = 0; i < window_size; ++i) {
        //     auto b = n | std::views::transform([fsx = fs[i]](auto x) { return std::cos(2 * std::numbers::pi * x) *  fsx; });
        //     window_[i] = static_cast<float>(1 + std::inner_product(a.begin(), a.end(), b.begin(), 0.0));

        //     auto b_front = n | std::views::transform([fsx = fs[i] - kTimeDelta](auto x) { return std::cos(2 * std::numbers::pi * x) *  fsx; });
        //     auto b_back = n | std::views::transform([fsx = fs[i] + kTimeDelta](auto x) { return std::cos(2 * std::numbers::pi * x) *  fsx; });
        //     dwindow_[i] = static_cast<float>(std::inner_product(b_front.begin(), b_front.end(), b_back.begin(), 0.0) / (2 * kTimeDelta));
        // }
        window_.resize(window_size);
        dwindow_.resize(window_size);
        taylorwin(window_.data(), dwindow_.data(), window_size, nb, -side_lobe_level);

        auto window_sum = std::accumulate(window_.cbegin(), window_.cend(), 0.0f);
        window_scale_ = 2.0f / window_sum;
        dwindow_scale_ = static_cast<float>(window_scale_ / (2 * std::numbers::pi));

        return window_size;
    }

    constexpr auto sq(auto x) {
        return x * x;
    }

    static constexpr auto M_PI = std::numbers::pi;

    void taylorwin(float* w, float* dw, unsigned n, unsigned nbar, double sll)
    {
        // Taylor window.
        //
        // Default Matlab parameters: nbar ==4, sll == -30.0.
        //
        // The Taylor window is cosine-window like, in that it is the sum of weighted
        // cosines of different periods.

        // sll is in dB(power).
        // Calculate the amplification factor, e.g. sll = -60 --> amplification = 1000.0

        const double amplification = pow(10.0, -sll / 20.0);

        const double a = acosh(amplification) / M_PI;

        const double a2 = sq(a);

        // Taylor pulse widening (dilation) factor.

        const double sp2 = sq(nbar) / (a2 + sq(nbar - 0.5));

        for (unsigned i = 0; i < n; ++i)
        {
            w[i] = 1.0; // Initial value.
        }

        constexpr auto time_delta = 0.0001;
        std::vector<double> front_val(n, 1.0);
        std::vector<double> back_val(n, 1.0);
        for (unsigned m = 1; m < nbar; ++m)
        {
            // Calculate Fm as a function of: m, sp2, a

            double numerator = 1.0;
            double denominator = 1.0;

            for (unsigned i = 1; i < nbar; ++i)
            {
                numerator *= (1.0 - sq(m) / (sp2 * (a2 + sq(i - 0.5))));
                if (i != m)
                {
                    denominator *= (1.0 - sq(m) / sq(i));
                }
            }

            const double Fm = -(numerator / denominator);

            // Add cosine term to each of the window components.

            for (unsigned i = 0; i < n; ++i)
            {
                const double x = 2 * M_PI * (i + 0.5) / n;
                const double front_x = 2 * M_PI * ((i + 0.5) / n - time_delta);
                const double back_x = 2 * M_PI * ((i + 0.5) / n + time_delta);
                w[i] += static_cast<float>(Fm * cos(m * x));
                front_val[i] += static_cast<float>(Fm * cos(m * front_x));
                back_val[i] += static_cast<float>(Fm * cos(m * back_x));
            }
        }

        for (unsigned i = 0; i < n; ++i)
        {
            dw[i] = static_cast<float>((back_val[i] - front_val[i]) / (2 * time_delta));
        }
    }

    int BlackManWin(float main_lobe_width) {
        auto main_width_in_bin = 2.0 * 1.727;
        auto n = static_cast<int>(std::ceil(main_width_in_bin / main_lobe_width));
        if (n % 2 == 0) ++n;

        window_.resize(n);
        dwindow_.resize(n);

        auto alpha = 0.17;
        auto a0 = (1.0 - alpha) / 2.0;
        auto a2 = alpha / 2.0;
        for (int i = 0; i < n; ++i) {
            auto t = static_cast<float>(i) / (n - 1.0);
            window_[i] = a0 - 0.5 * std::cos(std::numbers::pi * 2 * t) + a2 * std::cos(std::numbers::pi * 4 * t);
            dwindow_[i] = 0.5 * std::numbers::pi * 2 * std::sin(std::numbers::pi * 2 * t) - a2 * std::numbers::pi * 4 * std::sin(std::numbers::pi * 4 * t);
        }

        auto window_sum = std::accumulate(window_.cbegin(), window_.cend(), 0.0f);
        window_scale_ = 2.0f / window_sum;
        dwindow_scale_ = window_scale_ / (2 * std::numbers::pi);

        return n;
    }

    void Transform(const std::vector<float>& buffer) {
        assert(buffer.size() == WindowLen());
        xh_data_.resize(FftDataLen());
        xdh_data_.resize(FftDataLen());

        std::vector<float> real(FftDataLen());
        std::vector<float> imag(FftDataLen());
        std::vector<float> sample(fft_size);
        audiofft::AudioFFT fft;
        fft.init(fft_size);

        auto k = (WindowLen() - 1) / 2;
        std::ranges::transform(buffer | std::views::take(k), window_ | std::views::take(k), sample.begin() + fft_size - k, std::multiplies{});
        std::ranges::transform(buffer | std::views::drop(k), window_ | std::views::drop(k), sample.begin(), std::multiplies{});
        fft.fft(sample.data(), real.data(), imag.data());
        for (int i = 0; auto & cpx : xh_data_) {
            cpx = std::complex{ real[i] ,imag[i] } *window_scale_;
            ++i;
        }

        std::ranges::transform(buffer | std::views::take(k), dwindow_ | std::views::take(k), sample.begin() + fft_size - k, std::multiplies{});
        std::ranges::transform(buffer | std::views::drop(k), dwindow_ | std::views::drop(k), sample.begin(), std::multiplies{});
        fft.fft(sample.data(), real.data(), imag.data());
        for (int i = 0; auto & cpx : xdh_data_) {
            cpx = std::complex{ real[i],imag[i] } *dwindow_scale_;
            ++i;
        }
    }

    int WindowLen() const {
        return static_cast<int>(window_.size());
    }

    int FftDataLen() const {
        return fft_size / 2 + 1;
    }

    float CorrectFreqBin(int i) const {
        auto up = xdh_data_[i].imag() * xh_data_[i].real() - xdh_data_[i].real() * xh_data_[i].imag();
        auto down = std::norm(xh_data_[i]);
        float oversample = static_cast<float>(fft_size) / WindowLen();
        auto freq_c = -up * oversample / down;
        return i + freq_c;
    }

    float CorrectNorFreq(int i) const {
        return CorrectFreqBin(i) / fft_size;
    }

    float CorrectGain(int i) const {
        return std::abs(xh_data_[i]);
    }

    int FftSize() const { return fft_size; }

// private:
    float window_scale_{};
    float dwindow_scale_{};

    int fft_size = 8192;
    std::vector<float> window_;
    std::vector<float> dwindow_;
    std::vector<std::complex<float>> xh_data_;
    std::vector<std::complex<float>> xdh_data_;
};

int main() {
    std::ofstream out{ "fft.csv" };

    FuriesTransform f;
    auto len = f.TaylorWindow(70, 64.0/24000.0, 4);
    // auto len = f.KaiserWindow(100, 0.044922);
    out << std::format("window_len: {}, fft_size: {}\n", len, f.FftSize());
    for (int i = 0; i < len; ++i) {
        out << std::format("{},{}\n", f.window_[i], f.dwindow_[i]);
    }
    out.close();
}