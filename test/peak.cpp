#include <raylib.h>
#include <array>
#include <vector>
#include <format>
#include <gcem.hpp>
#include <AudioFFT.h>
#include <numbers>
#include <cmath>
#include <numeric>
#include <ranges>
#include <algorithm>
#include <complex>
#include <AudioFile.h>

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
        std::vector<float> dsample(fft_size);

        // 加窗和零相补0
        {
            auto k = (WindowLen() - 1) / 2;
            std::ranges::transform(buffer | std::views::take(k), window_ | std::views::take(k), sample.begin() + fft_size - k, std::multiplies{});
            std::ranges::transform(buffer | std::views::drop(k), window_ | std::views::drop(k), sample.begin(), std::multiplies{});
            std::ranges::transform(buffer | std::views::take(k), dwindow_ | std::views::take(k), dsample.begin() + fft_size - k, std::multiplies{});
            std::ranges::transform(buffer | std::views::drop(k), dwindow_ | std::views::drop(k), dsample.begin(), std::multiplies{});
        }

        {
            audiofft::AudioFFT fft;
            fft.init(fft_size);
            fft.fft(sample.data(), real.data(), imag.data());
            for (int i = 0; auto & cpx : xh_data_) {
                cpx = std::complex{ real[i] ,imag[i] } * window_scale_;
                ++i;
            }
            fft.fft(dsample.data(), real.data(), imag.data());
            for (int i = 0; auto & cpx : xdh_data_) {
                cpx = std::complex{ real[i],imag[i] } * dwindow_scale_;
                ++i;
            }
        }
    }

    int WindowLen() const {
        return static_cast<int>(window_.size());
    }

    int FftDataLen() const {
        return fft_size / 2 + 1;
    }

    float CorrectFreq(int i) const {
        auto up = xdh_data_[i].imag() * xh_data_[i].real() - xdh_data_[i].real() * xh_data_[i].imag();
        auto down = std::norm(xh_data_[i]);
        float oversample = static_cast<float>(fft_size) / WindowLen();
        auto freq_c = -up * oversample / down;
        return i + freq_c;
    }

    float CorrectNorFreq(int i) const {
        return CorrectFreq(i) / fft_size;
    }

    float CorrectGain(int i) const {
        return std::abs(xh_data_[i]);
    }

    int FftSize() const { return fft_size; }
private:
    float window_scale_{};
    float dwindow_scale_{};

    int fft_size = 8192;
    std::vector<float> window_;
    std::vector<float> dwindow_;
    std::vector<std::complex<float>> xh_data_;
    std::vector<std::complex<float>> xdh_data_;
};

constexpr auto kNumPartials = 256;

struct GainAndFreqPhase {
    float freq{};
    float freq2{};
    float gain_db{};
    float phase{};
};

struct ResynthsisFrames {
    enum class Type {
        kUnknow,
        kAudio,
        kImage
    };

    struct FftFrame {
        std::vector<GainAndFreqPhase> org_fftdatas;
        std::vector<GainAndFreqPhase> peaks;
    };

    void DuplicateExtraDataForLerp() {
        const auto last_frame = frames.back();
        frames.emplace_back(last_frame);
        frames.emplace_back(last_frame);
    }

    std::vector<FftFrame> frames;
    int num_frame{};
    float frame_interval_sample{};
    float base_freq{};
    Type source_type{ Type::kUnknow };
};

static constexpr float PitchToFreq(float pitch) {
    return gcem::exp(pitch * 0.05776226504666210911810267678818f) * 8.1758f;
}

static constexpr float DbToGain(float db) {
    return gcem::exp(0.11512925464970228420089957273422f * db);
}

static constexpr auto GainToDb(std::floating_point auto gain, std::floating_point auto min_db) {
    auto min_gain = DbToGain(min_db);
    if (gain <= min_gain) {
        return min_db;
    }
    return 20.0f * gcem::log10(gain);
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

static ResynthsisFrames CreateResynthsisFramesFromAudio(const std::vector<float>& sample,
                                                        float source_sample_rate,
                                                        float synthsis_sample_rate) {
    constexpr auto c2_freq = PitchToFreq(36.0f);
    constexpr auto kFFtHop = 256;

    // kasier window
    FuriesTransform transform;
    auto nor_freq_resolution = c2_freq / source_sample_rate; // 标准化频率分辨率
    int window_len = transform.KaiserWindow(70.0f, nor_freq_resolution);
    //int window_len = transform.BlackManWin(nor_freq_resolution);

    ResynthsisFrames audio_frames;
    audio_frames.source_type = ResynthsisFrames::Type::kAudio;
    audio_frames.frames.reserve(static_cast<size_t>(sample.size() / static_cast<float>(kFFtHop)));
    audio_frames.frame_interval_sample = kFFtHop / (source_sample_rate / synthsis_sample_rate);
    audio_frames.base_freq = c2_freq;

    auto read_pos = 0;
    std::vector<float> sample_buffer(window_len);
    while (read_pos < sample.size()) {
        std::ranges::fill(sample_buffer, 0.0f);
        if (read_pos + window_len <= sample.size()) {
            auto it = sample.begin() + read_pos;
            std::copy(it, it + window_len, sample_buffer.begin());
        }
        else {
            // todo: solve the click
            const auto num_samples_can_read = sample.size() - read_pos;
            const auto center_idx = (num_samples_can_read + 1) / 2;
            auto begin_idx = center_idx - num_samples_can_read / 2;
            for (int i = 0; i < num_samples_can_read; ++i) {
                sample_buffer[begin_idx++] = sample.at(read_pos + i);
            }
        }
        read_pos += kFFtHop;

        // 加窗
        transform.Transform(sample_buffer);

        ResynthsisFrames::FftFrame new_frame;

        std::vector<GainAndFreqPhase> high_resolution_infos(transform.FftDataLen());
        for (int i = 0; auto & s : high_resolution_infos) {
            float e = i;
            s.freq2 = e / transform.FftSize() * source_sample_rate;
            s.freq = transform.CorrectNorFreq(i) * source_sample_rate;
            s.gain_db = GainToDb(transform.CorrectGain(i), -96.0f);
            s.phase = 0.0f;
            ++i;
        }

        new_frame.org_fftdatas = high_resolution_infos; // debug

        for (int i = 0; i < high_resolution_infos.size() - 1; ++i) {
            /*if (!(high_resolution_infos[i + 1].gain_db > high_resolution_infos[i].gain_db
                  && high_resolution_infos[i + 1].gain_db > high_resolution_infos[i + 2].gain_db)) {
                continue;
            }

            if (high_resolution_infos[i + 1].gain_db < -60.0f) {
                continue;
            }

            new_frame.peaks.push_back(high_resolution_infos[i+1]);*/

            if (!(transform.CorrectFreq(i) > i && transform.CorrectFreq(i + 1) < i + 1)) {
                continue;
            }

            if (transform.CorrectGain(i) > transform.CorrectGain(i + 1)) {
                if (high_resolution_infos[i].gain_db > -60.0f) {
                    new_frame.peaks.push_back(high_resolution_infos[i]);
                }
            }
            else {
                if (high_resolution_infos[i + 1].gain_db > -60.0f) {
                    new_frame.peaks.push_back(high_resolution_infos[i + 1]);
                }
            }
        }

        std::ranges::sort(new_frame.peaks, std::greater{}, &GainAndFreqPhase::gain_db);
        std::vector<GainAndFreqPhase> peaks;
        for (auto peak : new_frame.peaks) {
            if (std::ranges::find_if(peaks, [f = c2_freq,peak](GainAndFreqPhase p) -> bool {
                return peak.freq > p.freq - f && peak.freq < p.freq + f;
            }) == peaks.cend()) {
                peaks.push_back(peak);
            }
        }
        new_frame.peaks = std::move(peaks);

        audio_frames.frames.emplace_back(std::move(new_frame));
    }

    audio_frames.num_frame = static_cast<int>(audio_frames.frames.size());
    audio_frames.DuplicateExtraDataForLerp();
    return audio_frames;
}

int main(void) {
    InitWindow(800, 600, "additive synth");
    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(480);
    SetTargetFPS(60);               // Set our game to run at 30 frames-per-second

    auto audio = AudioFile<float>{ R"(C:\Users\Kawai\Desktop\test.wav)" };
    auto frames = CreateResynthsisFramesFromAudio(audio.samples.front(),
                                                  audio.getSampleRate(),
                                                  48000.0f);
    const auto sample_rate = static_cast<float>(audio.getSampleRate());
    int current_display_frame = 0;
    int num_display_frame = 8;
    float freq_max = 1000.0f;
    float region_width = GetScreenWidth();
    float region_height = GetScreenHeight() / num_display_frame;

    // Main game loop
    while (!WindowShouldClose()) { // Detect window close button or ESC key
        // drawing
        BeginDrawing();
        ClearBackground(BLACK);

        if (IsKeyDown(KEY_DOWN)) {
            ++current_display_frame;
        }
        if (IsKeyDown(KEY_UP)) {
            --current_display_frame;
            current_display_frame = std::max(current_display_frame, 0);
        }
        if (IsKeyDown(KEY_LEFT)) {
            freq_max = std::max(500.0f, freq_max - 100.0f);
        }
        if (IsKeyDown(KEY_RIGHT)) {
            freq_max = std::min(20000.0f, freq_max + 100.0f);
        }

        for (int i = 0; i < kNumPartials; ++i) {
            auto f = (i + 1.0) * frames.base_freq;
            if (f > freq_max) {
                break;
            }
            auto x = f / freq_max * region_width;
            //DrawLine(x, 0, x, GetScreenHeight(), WHITE);
            x = (f + frames.base_freq * 0.5f) / freq_max * region_width;
            DrawLine(x, 0, x, GetScreenHeight(), RED);
            x = (f - frames.base_freq * 0.5f) / freq_max * region_width;
            DrawLine(x, 0, x, GetScreenHeight(), RED);
        }

        for (int i = 0; i < num_display_frame; ++i) {
            DrawRectangleLines(0, region_height * i, region_width, region_height, RED);
            if (current_display_frame + i >= frames.num_frame || current_display_frame + i < 0) {
            }
            else {
                constexpr auto gain_db_min = -60.0f;

                auto mouse_x = GetMousePosition().x / GetScreenWidth();
                DrawText(std::format("{:.1f} hz", mouse_x * freq_max).c_str(), 0, region_height * i, 12, WHITE);

                const auto& frame = frames.frames[i + current_display_frame];
                for (const auto& fft_data : frame.org_fftdatas) {
                    auto x = fft_data.freq / freq_max * region_width;
                    auto y = region_height * (i + 1) - region_height * (std::clamp(fft_data.gain_db, gain_db_min, 6.0f) / (6 - gain_db_min) + 1.0f);
                    //DrawPixel(x, y, GREEN);
                    DrawText("*", x, y - 4, 4, GREEN);
                    x = fft_data.freq2 / freq_max * region_width;
                    DrawText("+", x, y - 4, 4, RED);
                }

                for (const auto& peak : frame.peaks) {
                    auto fft_data = peak;
                    auto x = fft_data.freq / freq_max * region_width;
                    auto y = region_height * (i + 1) - region_height * (std::clamp(fft_data.gain_db, gain_db_min, 6.0f) / (6 - gain_db_min) + 1.0f);
                    //DrawPixel(x, y, GREEN);
                    DrawText("o", x, y - 4, 4, WHITE);
                }
            }
        }

        EndDrawing();
    }
    CloseWindow();              // Close window and OpenGL context
}