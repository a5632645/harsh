#include "synth.h"
#include "synth.h"

#include "resynthsis/window.h"
#include "param/standard_param.h"
#include "AudioFFT.h"
#include <mutex>
#include <numeric>
#include "utli/convert.h"

namespace mana {
Synth::Synth(std::shared_ptr<ParamCreator> creator, size_t num_osc)
    : synth_params_(creator)
    , num_oscillor_(num_osc) {
    mono_modu_params_ = std::make_unique<ModulableParams>(synth_params_,
                                                          std::vector{ ModulationType::kModulable, ModulationType::kPoly });
    mono_modulator_bank_.PrepareParams(*mono_modu_params_);
    timefx_chain_.PrepareParams(*mono_modu_params_);

    m_oscillators.reserve(num_oscillor_);
    for (int i = 0; i < num_oscillor_; ++i) {
        m_oscillators.emplace_back(*this);
    }

    output_gain_ = mono_modu_params_->GetModuFloatParam("output_gain");
    synth_params_.AddModulationListener(this);
}

void Synth::NoteOn(int note, float velocity) {
    mono_modulator_bank_.OnNoteOn(note);
    timefx_chain_.OnNoteOn(note);
    // rr one time
    for (size_t i = 0; i < num_oscillor_; ++i) {
        if (!m_oscillators[m_rrPosition].IsPlaying()) {
            m_oscillators[m_rrPosition].NoteOn(note, velocity);
            m_rrPosition = (m_rrPosition + 1) % num_oscillor_;
            return;
        }
        m_rrPosition = (m_rrPosition + 1) % num_oscillor_;
    }
    // if not found
    // stop rr osciilor
    m_oscillators[m_rrPosition].Reset();
    m_oscillators[m_rrPosition].NoteOff();
    // set current note
    m_oscillators[m_rrPosition].NoteOn(note, velocity);
    m_rrPosition = (m_rrPosition + 1) % num_oscillor_;
}

void Synth::NoteOff(int note, float velocity) {
    for (Oscillor& o : m_oscillators) {
        if (o.getMidiNote() == note) {
            o.NoteOff();
        }
    }
}

void Synth::Render(float* buffer, int num_frame) {
    for (Oscillor& o : m_oscillators) {
        if (!o.IsPlaying()) {
            continue;
        }

        for (int i = 0; i < num_frame; ++i) {
            buffer[i] += o.SrTick();
        };
    }
    timefx_chain_.Process(buffer, num_frame);

    auto output_gain = utli::DbToGain(output_gain_->GetValue());
    smooth_output_gain_.SetTarget(output_gain);
    for (int i = 0; i < num_frame; ++i) {
        buffer[i] *= smooth_output_gain_.TickNext();
    }
}

void Synth::Init(size_t buffer_size, float sample_rate, float update_rate) {
    sample_rate_ = sample_rate;
    update_skip_ = static_cast<int>(std::round(sample_rate / update_rate));
    update_rate_ = sample_rate / static_cast<float>(update_skip_);

    mono_modulator_bank_.Init(sample_rate, update_rate_);
    timefx_chain_.Init(sample_rate, update_rate_);
    smooth_output_gain_.SetSmooth(sample_rate);
    for (Oscillor& o : m_oscillators) {
        o.Init(buffer_size, sample_rate, update_rate_, update_skip_);
    }
}

void Synth::update_state(int step) {
    mono_modu_params_->UpdateParams();
    mono_modulator_bank_.OnUpdateTick();
    timefx_chain_.OnUpdateTick();
    for (Oscillor& o : m_oscillators) {
        o.update_state(step);
    }
}

const Oscillor& Synth::GetDisplayOscillor() const {
    auto idx = static_cast<int>(m_rrPosition) - 1;
    auto last_played = (idx + num_oscillor_) % num_oscillor_;
    for (size_t i = 0; i < num_oscillor_; ++i) {
        idx += num_oscillor_;
        idx %= num_oscillor_;
        if (m_oscillators[idx].IsPlaying()) {
            return m_oscillators[idx];
        }
        --idx;
    }

    return m_oscillators[last_played];
}

static constexpr float PhaseWrap(float p) {
    while (p > std::numbers::pi_v<float>) {
        p -= std::numbers::pi_v<float> *2.0f;
    }
    while (p < -std::numbers::pi_v<float>) {
        p += std::numbers::pi_v<float> *2.0f;
    }
    return p;
}

void Synth::SetResynthsisFrames(ResynthsisFrames new_frame) {
    resynthsis_frames_ = std::move(new_frame);
}

std::pair<bool, ModulationConfig*> Synth::CreateModulation(std::string_view modulator, std::string_view param) {
    auto exist_modu = synth_params_.FindModulation(modulator, param);
    if (exist_modu != nullptr) {
        return { false, nullptr };
    }

    auto new_modulation_cfg = std::make_shared<ModulationConfig>();
    new_modulation_cfg->modulator_id = modulator;
    new_modulation_cfg->param_id = param;
    new_modulation_cfg->enable = true;
    synth_params_.AddModulation(new_modulation_cfg);
    return { true, new_modulation_cfg.get() };
}

void Synth::RemoveModulation(ModulationConfig& config) {
    synth_params_.RemoveModulation(config);
}

class FuriesTransform {
public:
    FuriesTransform(ResynthsisOption& option) : option_(option) {}

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
        n = option_.custom_win_len_ ? option_.custom_win_len_ : n;
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
        window_size = option_.custom_win_len_ ? option_.custom_win_len_ : window_size;
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
        n = option_.custom_win_len_ ? option_.custom_win_len_ : n;
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

private:
    ResynthsisOption& option_;

    float window_scale_{};
    float dwindow_scale_{};

    int fft_size = 8192;
    std::vector<float> window_;
    std::vector<float> dwindow_;
    std::vector<std::complex<float>> xh_data_;
    std::vector<std::complex<float>> xdh_data_;
};

ResynthsisFrames Synth::CreateResynthsisFramesFromAudio(
    const std::vector<float>& sample,
    float source_sample_rate,
    ResynthsisOption option
) const {
    constexpr auto c2_freq = utli::PitchToFreq(36.0f);
    constexpr auto analyze_fft_size = 8192;
    constexpr auto kFFtHop = 256;

    // option
    if (!option.custom_freq_res_)
        option.frequency_resolution_ = c2_freq;
    if (!option.custom_side_lobe_level_)
        option.side_lobe_level_ = 80.0f;
    if (!option.custom_peak_filter_level_)
        option.peak_filter_level_ = -60.0f;
    if (!option.custom_smooth_time_)
        option.smooth_time_ = 10.0f;

    FuriesTransform transform{option};
    int win_len = 0;

    switch (option.window_) {
    case ResynthsisOption::Window::kBlackman:
        win_len = transform.BlackManWin(c2_freq / source_sample_rate);
        break;
    case ResynthsisOption::Window::kKaiser:
        win_len = transform.KaiserWindow(option.side_lobe_level_, c2_freq / source_sample_rate);
        break;
    case ResynthsisOption::Window::kTaylor:
        win_len = transform.TaylorWindow(option.side_lobe_level_, c2_freq / source_sample_rate / 2, 4);
        break;
    default:
        assert(false);
        break;
    }
    const int num_frames = static_cast<int>(std::ceil((sample.size() - win_len) / static_cast<float>(kFFtHop)));

    ResynthsisFrames audio_frames;
    audio_frames.source_type = ResynthsisFrames::Type::kAudio;
    audio_frames.frames.reserve(num_frames);
    audio_frames.frame_interval_sample = kFFtHop / (source_sample_rate / sample_rate_);
    audio_frames.base_freq = c2_freq;

    /*
    *    zero | zero | gain
    *           idx    curr
    */
    std::vector<std::vector<int>> transit_discontinue(kNumPartials);
    /*
    *    gain | gain | zero
    *           idx    curr
    */
    std::vector<std::vector<int>> silence_discontinue(kNumPartials);
    std::array<bool, kNumPartials> last_frame_peak_state{};

    auto read_pos = 0;
    auto max_db = -999.0f;
    int frame_idx = 0;
    std::vector<float> sample_buffer(win_len);
    for (int _frame = 0; _frame < num_frames; ++_frame) {
        std::ranges::fill(sample_buffer, 0.0f);
        if (read_pos + win_len <= sample.size()) {
            auto it = sample.begin() + read_pos;
            std::copy(it, it + win_len, sample_buffer.begin());
        }
        else {
            // todo: solve the click
            const auto num_samples_can_read = sample.size() - read_pos;
            const auto center_idx = (num_samples_can_read + 1) / 2;
            auto begin_idx = center_idx - num_samples_can_read / 2;
            for (int i = 0; i < num_samples_can_read; ++i) {
                sample_buffer[begin_idx++] = sample[read_pos + i];
            }
        }
        read_pos += kFFtHop;
        transform.Transform(sample_buffer);

        struct GainAndFreqPhase {
            float freq{};
            float gain_db{};
            float gain{};
            float phase{};
        };
        std::vector<GainAndFreqPhase> high_resolution_infos(transform.FftDataLen());
        for (int i = 0; auto & s : high_resolution_infos) {
            s.freq = transform.CorrectNorFreq(i) * source_sample_rate;
            s.gain_db = utli::GainToDb(transform.CorrectGain(i));
            s.gain = transform.CorrectGain(i);
            s.phase = 0.0f;
            ++i;
        }

        std::vector<GainAndFreqPhase> peaks;
        for (int i = 0; i < high_resolution_infos.size() - 1; ++i) {
            if (!(transform.CorrectFreqBin(i) > i && transform.CorrectFreqBin(i + 1) < i + 1)) {
                continue;
            }

            auto tmp = high_resolution_infos[i + 1];
            if (transform.CorrectGain(i) > transform.CorrectGain(i + 1)) {
                tmp = high_resolution_infos[i];
            }

            if (tmp.gain_db < option.peak_filter_level_) {
                continue;
            }
            peaks.push_back(tmp);
        }

        std::ranges::sort(peaks, std::greater{}, &GainAndFreqPhase::gain_db);
        std::vector<GainAndFreqPhase> filter_peaks;
        for (auto peak : peaks) {
            if (std::ranges::find_if(filter_peaks, [f = c2_freq, peak](GainAndFreqPhase p) -> bool {
                return peak.freq > p.freq - f && peak.freq < p.freq + f;
            }) == filter_peaks.cend()) {
                filter_peaks.push_back(peak);
            }
        }
        std::ranges::sort(filter_peaks, std::less{}, &GainAndFreqPhase::freq);

        ResynthsisFrames::FftFrame new_frame;
        for (int i = 0; i < kNumPartials; ++i) {
            auto max_search_freq_diff = c2_freq * 0.5f;
            auto res_partial_freq = c2_freq * (i + 1.0f);
            auto min_freq = res_partial_freq - max_search_freq_diff;
            auto max_freq = res_partial_freq + max_search_freq_diff;
            auto begin_it = std::ranges::find_if(filter_peaks, [min_freq](auto d) {return d.freq >= min_freq; });
            auto end_it = std::ranges::find_if(filter_peaks, [max_freq](auto d) {return d.freq > max_freq; });
            auto max_one_it = std::max_element(begin_it, end_it, [](auto largest, auto curr) {
                return curr.gain_db > largest.gain_db;
            });

            if (max_one_it == end_it) {
                new_frame.db_gains[i] = -300.0f;
                new_frame.ratio_diffs[i] = 0.0f;
            }
            else {
                auto max_one = *max_one_it;
                auto ratio_diff = (max_one.freq - res_partial_freq) / c2_freq;
                new_frame.db_gains[i] = max_one.gain_db;
                new_frame.ratio_diffs[i] = ratio_diff;
            }

            bool has_peak = max_one_it != end_it;
            if (frame_idx != 0) {
                if (!last_frame_peak_state[i] && has_peak) {
                    transit_discontinue[i].push_back(frame_idx - 1);
                }
                if (last_frame_peak_state[i] && !has_peak) {
                    silence_discontinue[i].push_back(frame_idx - 1);
                }
            }
            last_frame_peak_state[i] = has_peak;
        }

        max_db = std::max(max_db, *std::ranges::max_element(new_frame.db_gains));
        audio_frames.frames.emplace_back(std::move(new_frame));
        ++frame_idx;
    }

    const auto fade_samples = sample_rate_ * option.smooth_time_ / 1000.0f;
    const auto fade_frames = static_cast<int>(std::ceil(fade_samples / kFFtHop));
    const auto slope = 60.0f / (fade_frames + 1.0f);
    for (int i = 0; i < kNumPartials; ++i) {
        const auto& transit_dis = transit_discontinue[i];
        const auto& silence_dis = silence_discontinue[i];
        for (int idx : transit_dis) {
            auto transit_gain = audio_frames.frames[idx + 1].db_gains[i];
            auto transit_ratio = audio_frames.frames[idx + 1].ratio_diffs[i];

            for (int j = 0; j < fade_frames; ++j) {
                int fidx = idx - j;
                if (fidx < 0) {
                    break;
                }
                auto fade_gain = transit_gain - (j + 1.0f) * slope;
                if (fade_gain > audio_frames.frames[fidx].db_gains[i]) {
                    audio_frames.frames[fidx].db_gains[i] = fade_gain;
                    audio_frames.frames[fidx].ratio_diffs[i] = transit_ratio;
                }
            }
        }
        for (int idx : silence_dis) {
            auto silence_gain = audio_frames.frames[idx].db_gains[i];
            auto silence_ratio = audio_frames.frames[idx].ratio_diffs[i];
            for (int j = 0; j < fade_frames; ++j) {
                int fidx = idx + j + 1;
                if (fidx >= num_frames) {
                    break;
                }
                auto fade_gain = silence_gain - (j + 1.0f) * slope;
                if (fade_gain > audio_frames.frames[fidx].db_gains[i]) {
                    audio_frames.frames[fidx].db_gains[i] = fade_gain;
                    audio_frames.frames[fidx].ratio_diffs[i] = silence_ratio;
                }
            }
        }
    }

    auto gain_level_up = 0.0f - max_db;
    for (auto& frame : audio_frames.frames) {
        for (auto& level : frame.db_gains) {
            level += gain_level_up;
        }
    }

    audio_frames.num_frame = num_frames;
    audio_frames.level_up_db = gain_level_up;
    audio_frames.DuplicateExtraDataForLerp();
    return audio_frames;
}

ResynthsisFrames Synth::CreateResynthsisFramesFromImage(std::unique_ptr<ImageBase> image_in, bool stretch_image) {
    ResynthsisFrames image_frame;

    /*
    * R nothing
    * G gain       [0,255] map to [-60,0]dB
    * B ratio_diff [0,255] map to [-1, 1]
    * simulate harmor's audio convert to image resynthsis mode
    */
    const auto w = image_in->GetWidth();
    const auto h = image_in->GetHeight();
    const auto max_green = image_in->GetMaxGreen();
    const auto valid_gain = max_green != 0;
    image_frame.frames.resize(w);
    image_frame.frame_interval_sample = 256; // equal to harmor
    constexpr auto c2_freq = utli::PitchToFreq(36.0f);
    constexpr auto kImgMaxDb = 0.0f;
    constexpr auto kImgMinDb = -60.0f;
    image_frame.base_freq = c2_freq;
    image_frame.source_type = ResynthsisFrames::Type::kImage;
    auto max_db = -999.0f;

    constexpr auto db6_level_down_table = []() {
        std::array<float, kNumPartials> out{};
        for (int i = 0; i < kNumPartials; ++i)
            out[i] = utli::GainToDb(1.0L / (i + 1.0L));
        return out;
    }();

    // todo: keep align
    auto y_loop = kNumPartials;
    if (!stretch_image)
        y_loop = std::min(y_loop, h);

    for (int x = 0; x < w; ++x) {
        auto& frame = image_frame.frames[x];

        std::ranges::fill(frame.db_gains, -300.0f);
        for (int y = 0; y < y_loop; ++y) {
            auto image_y_idx = y_loop - y - 1;
            if (stretch_image) {
                auto y_nor = static_cast<float>(y) / static_cast<float>(kNumPartials - 1);
                image_y_idx = std::clamp(static_cast<int>(h - y_nor * h), 0, h - 1);
            }
            auto pixel = image_in->GetPixel(x, image_y_idx);

            // map g to gain
            if (valid_gain) {
                if (pixel.g != 0) {
                    auto g_div = static_cast<float>(pixel.g) / max_green;
                    auto map_db = std::lerp(kImgMinDb, kImgMaxDb, g_div);
                    //auto db = std::lerp(kImgMinDb, kImgMaxDb, static_cast<float>(pixel.g - 16.0f) / 256.0f) + db6_level_down_table[y];
                    auto db = map_db + db6_level_down_table[y];
                    frame.db_gains[y] = db;
                    max_db = std::max(db, max_db);
                }
                else {
                    frame.db_gains[y] = -300.0f;
                }
            }

            // map b to ratio diff
            auto ratio_diff = 2.0f * static_cast<float>(pixel.b) / 255.0f - 1.0f;
            frame.ratio_diffs[y] = ratio_diff;
        }
    }

    if (valid_gain) {
        auto gain_level_db = 0.0f - max_db;
        for (int i = 0; auto & frame : image_frame.frames) {
            for (int j = 0; auto & level : frame.db_gains) {
                level += gain_level_db;
            }
        }
        image_frame.level_up_db = gain_level_db;
    }
    else {
        for (auto& f : image_frame.frames) {
            std::ranges::copy(db6_level_down_table, f.db_gains.begin());
        }
        image_frame.level_up_db = 0.0f;
    }

    image_frame.num_frame = static_cast<int>(image_frame.frames.size());
    image_frame.DuplicateExtraDataForLerp();
    return image_frame;
}

nlohmann::json Synth::SaveState() const {
    auto s = synth_params_.SaveState();
    s["timefx_order"] = timefx_chain_.SaveState();
    return s;
}

void Synth::LoadState(const nlohmann::json& json) {
    synth_params_.LoadState(json);
    timefx_chain_.LoadState(json["timefx_order"]);
}

void Synth::OnModulationAdded(std::shared_ptr<ModulationConfig> config) {
    auto* p = synth_params_.GetParamBank().GetParamPtr(config->param_id);
    assert(p != nullptr);
    assert(p->GetModulationType() != ModulationType::kDisable);

    // mono
    mono_modu_params_->CreateModulation(mono_modulator_bank_.GetModulatorPtr(config->modulator_id), config);
    if (p->GetModulationType() == ModulationType::kModulable) {
        return;
    }

    // poly
    for (auto& osc : m_oscillators) {
        osc.CreateModulation(config);
    }
}

void Synth::OnModulationRemoved(std::string_view modulator_id, std::string_view param_id) {
    auto* p = synth_params_.GetParamBank().GetParamPtr(param_id);
    assert(p != nullptr);
    assert(p->GetModulationType() != ModulationType::kDisable);

    // mono
    mono_modu_params_->RemoveModulation(modulator_id, param_id);
    if (p->GetModulationType() == ModulationType::kModulable) {
        return;
    }

    // poly
    for (auto& osc : m_oscillators) {
        osc.RemoveModulation(modulator_id, param_id);
    }
}

void Synth::OnModulationCleared() {
    mono_modu_params_->ClearModulations();
    for (auto& osc : m_oscillators) {
        osc.ClearModulations();
    }
}
}