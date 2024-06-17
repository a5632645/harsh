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
    m_oscillators.reserve(num_oscillor_);
    for (int i = 0; i < num_oscillor_; ++i) {
        m_oscillators.emplace_back(*this);
    }

    output_gain_ = synth_params_.GetParamBank().GetParamPtr("output_gain");
}

void Synth::NoteOn(int note, float velocity) {
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
    m_oscillators.at(m_rrPosition).NoteOff();
    // set current note
    m_oscillators.at(m_rrPosition).NoteOn(note, velocity);
    m_rrPosition = (m_rrPosition + 1) % num_oscillor_;
}

void Synth::NoteOff(int note, float velocity) {
    for (Oscillor& o : m_oscillators) {
        if (o.getMidiNote() == note) {
            o.NoteOff();
        }
    }
}

void Synth::Render(size_t numFrame) {
    std::ranges::fill(audio_buffer_, 0.0F);

    int write_pos = 0;
    int left = static_cast<int>(numFrame);

    auto render_func = [&](int num) mutable {
        for (Oscillor& o : m_oscillators) {
            if (!o.IsPlaying()) {
                continue;
            }

            auto write_iter = audio_buffer_.begin() + write_pos;
            for (int i = 0; i < num; ++i) {
                *write_iter++ += o.SrTick();
            };
        }
        write_pos += num;
    };

    while (left != 0) {
        if (left >= update_counter_) {
            left -= update_counter_;
            render_func(update_counter_);
            update_counter_ = update_skip_;
            update_state(update_skip_);
        }
        else {
            render_func(left);
            update_counter_ -= left;
            break;
        }
    }

    auto output_gain = utli::DbToGain(output_gain_->GetValue());
    std::ranges::transform(audio_buffer_, audio_buffer_.begin(), [output_gain](float v) {return v * output_gain; });
}

void Synth::Render(float* buffer, int num_frame) {
    auto output_gain = utli::DbToGain(output_gain_->GetValue());

    auto ptr_begin = buffer;
    for (Oscillor& o : m_oscillators) {
        if (!o.IsPlaying()) {
            continue;
        }

        for (int i = 0; i < num_frame; ++i) {
            buffer[i] += o.SrTick();
        };
    }

    std::transform(ptr_begin, ptr_begin + num_frame, ptr_begin, [output_gain](float v) {return v * output_gain; });
}

void Synth::Init(size_t buffer_size, float sample_rate, float update_rate) {
    sample_rate_ = sample_rate;
    update_skip_ = static_cast<int>(std::round(sample_rate / update_rate));
    update_rate_ = sample_rate / static_cast<float>(update_skip_);

    audio_buffer_.resize(buffer_size);
    for (Oscillor& o : m_oscillators) {
        o.Init(buffer_size, sample_rate, update_rate_, update_skip_);
    }
}

void Synth::update_state(int step) {
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
    synth_params_.AddModulation(new_modulation_cfg);

    for (auto& osc : m_oscillators) {
        osc.CreateModulation(param, modulator, new_modulation_cfg.get());
    }

    return { true, new_modulation_cfg.get() };
}

void Synth::RemoveModulation(ModulationConfig& config) {
    for (auto& osc : m_oscillators) {
        osc.RemoveModulation(config);
    }

    synth_params_.RemoveModulation(config);
}

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
        twindow_ = window_;
        ApplyTimeRamp(twindow_);

        auto window_sum = std::accumulate(window_.cbegin(), window_.cend(), 0.0f);
        window_scale_ = 2.0f / window_sum;
        twindow_scale_ = window_scale_;
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
        twindow_ = window_;
        ApplyTimeRamp(twindow_);

        auto window_sum = std::accumulate(window_.cbegin(), window_.cend(), 0.0f);
        window_scale_ = 2.0f / window_sum;
        twindow_scale_ = window_scale_;
        dwindow_scale_ = window_scale_ / (2 * std::numbers::pi);

        return n;
    }

    void Transform(const std::vector<float>& buffer) {
        assert(buffer.size() == WindowLen());
        xh_data_.resize(FftDataLen());
        xdh_data_.resize(FftDataLen());
        xth_data_.resize(FftDataLen());

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

        //std::ranges::transform(buffer | std::views::take(k), twindow_ | std::views::take(k), sample.begin() + fft_size - k, std::multiplies{});
        //std::ranges::transform(buffer | std::views::drop(k), twindow_ | std::views::drop(k), sample.begin(), std::multiplies{});
        //fft.fft(sample.data(), real.data(), imag.data());
        //for (int i = 0; auto & cpx : xth_data_) {
        //    cpx = std::complex{ real[i],imag[i] } *twindow_scale_;
        //    ++i;
        //}
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

    float CorrectTimeInSamples(int i) const {
        auto up = xth_data_[i].imag() * xh_data_[i].real() - xth_data_[i].real() * xh_data_[i].imag();
        auto down = std::norm(xh_data_[i]);
        return up / down;
    }

    int FftSize() const { return fft_size; }
private:
    void ApplyTimeRamp(std::vector<float>& win) {
        auto center = (win.size() - 1.0f) * 0.5f;
        for (int i = 0; auto & s : win) {
            s *= (i - center);
        }
    }

    float window_scale_{};
    float dwindow_scale_{};
    float twindow_scale_{};

    int fft_size = 8192;
    std::vector<float> window_;
    std::vector<float> dwindow_;
    std::vector<float> twindow_;
    std::vector<std::complex<float>> xh_data_;
    std::vector<std::complex<float>> xdh_data_;
    std::vector<std::complex<float>> xth_data_;
};

ResynthsisFrames Synth::CreateResynthsisFramesFromAudio(const std::vector<float>& sample, float source_sample_rate) const {
    constexpr auto c2_freq = utli::cp::PitchToFreq(36.0f);
    constexpr auto analyze_fft_size = 8192;
    constexpr auto kFFtHop = 256;
    constexpr auto kThreadshoud = -60.0f;
    constexpr auto kCompressRange = 10.0f;
    constexpr auto kCompressThreadShound = kThreadshoud + kCompressRange;

    FuriesTransform transform;
    //int win_len = transform.KaiserWindow(65.0f, c2_freq * 0.9f / source_sample_rate);
    const int win_len = transform.BlackManWin(c2_freq * 2.0f / source_sample_rate);
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
            s.gain_db = utli::GainToDb<-300.0f>(transform.CorrectGain(i));
            s.gain = transform.CorrectGain(i);
            s.phase = 0.0f;
            ++i;
        }

        std::vector<GainAndFreqPhase> peaks;
        for (int i = 0; i < high_resolution_infos.size() - 1; ++i) {
            if (!(transform.CorrectFreqBin(i) > i && transform.CorrectFreqBin(i + 1) < i + 1)) {
                continue;
            }

            //if (transform.CorrectTimeInSamples(i) < 0) {
            //    continue;
            //}

            auto tmp = high_resolution_infos[i + 1];
            if (transform.CorrectGain(i) > transform.CorrectGain(i + 1)) {
                tmp = high_resolution_infos[i];
            }

            if (tmp.gain_db < kThreadshoud) {
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

    constexpr auto kFadeTime = 20; // ms
    const auto fade_samples = sample_rate_ * kFadeTime / 1000.0f;
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
    constexpr auto c2_freq = utli::cp::PitchToFreq(36.0f);
    constexpr auto kImgMaxDb = 0.0f;
    constexpr auto kImgMinDb = -60.0f;
    image_frame.base_freq = c2_freq;
    image_frame.source_type = ResynthsisFrames::Type::kImage;
    auto max_db = -999.0f;

    constexpr auto db6_level_down_table = []() {
        std::array<float, kNumPartials> out{};
        for (int i = 0; i < kNumPartials; ++i)
            out[i] = utli::cp::GainToDb(1.0L / (i + 1.0L), -300.0L);
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
}