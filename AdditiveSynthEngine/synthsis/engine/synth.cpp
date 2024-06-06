#include "synth.h"
#include "synth.h"

#include "resynthsis/window.h"
#include "param/standard_param.h"
#include "AudioFFT.h"
#include <mutex>
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

static float Db2Gain(float db) {
    return std::exp(0.11512925464970228420089957273422f * db);
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
    //{
    //    auto existed_it = std::ranges::find_if(modulation_configs_, [modulator, param](std::shared_ptr<ModulationConfig>& cfg) {
    //        return cfg->modulator_id == modulator && cfg->param_id == param;
    //    });
    //    if (existed_it != modulation_configs_.cend()) {
    //        return { false, nullptr };
    //    }
    //}
    auto exist_modu = synth_params_.FindModulation(modulator, param);
    if (exist_modu != nullptr) {
        return { false, nullptr };
    }

    auto new_modulation_cfg = std::make_shared<ModulationConfig>();
    new_modulation_cfg->modulator_id = modulator;
    new_modulation_cfg->param_id = param;
    //modulation_configs_.emplace_back(new_modulation_cfg);
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

    //auto it = std::ranges::find_if(modulation_configs_, [config](std::shared_ptr<ModulationConfig>& cfg) {
    //    return cfg->modulator_id == config.modulator_id
    //        && cfg->param_id == config.param_id;
    //});
    //modulation_configs_.erase(it);
    synth_params_.RemoveModulation(config);
}

static constexpr auto analyze_fft_size = 2048;
static constexpr auto sample_fft_size = 1024;
static constexpr auto kFFtHop = 256; // equal to harmor
ResynthsisFrames Synth::CreateResynthsisFramesFromAudio(const std::vector<float>& sample, float source_sample_rate) const {
    audiofft::AudioFFT fft;
    Window<float> window;
    fft.init(analyze_fft_size);
    window.Init(sample_fft_size);

    const auto sample_rate_ratio = source_sample_rate / sample_rate_;
    const auto c2_freq = std::exp2(36.0f / 12.0f) * 8.1758f;
    auto num_frame = static_cast<size_t>((sample.size() - static_cast<float>(analyze_fft_size)) / static_cast<float>(kFFtHop));
    ResynthsisFrames audio_frames;
    audio_frames.frames.reserve(num_frame);
    audio_frames.frame_interval_sample = kFFtHop / sample_rate_ratio;
    audio_frames.base_freq = c2_freq;

    auto read_pos = 0;
    auto max_gain = 0.0f;
    const auto num_bin = audiofft::AudioFFT::ComplexSize(analyze_fft_size);
    const auto max_search_freq_diff = c2_freq;
    std::vector<float> curr_phases(num_bin);
    std::vector<float> last_frame_phases(num_bin - 1); // do not contain dc
    std::vector<float> temp_gains(num_bin);
    std::vector<float> real(num_bin);
    std::vector<float> imag(num_bin);
    std::vector<float> fft_buffer(analyze_fft_size);
    while (num_frame--) {
        std::ranges::fill(fft_buffer, 0.0f); // pad 0 expand to analyze fft size
        if (read_pos + sample_fft_size <= sample.size()) {
            auto it = sample.begin() + read_pos;
            std::copy(it, it + sample_fft_size, fft_buffer.begin());
        }
        else {
            const auto num_samples_can_read = sample.size() - read_pos;
            for (int i = 0; i < num_samples_can_read; ++i) {
                fft_buffer[i] = sample.at(read_pos + i);
            }
        }

        window.ApplyWindow(fft_buffer);
        fft.fft(fft_buffer.data(), real.data(), imag.data());

        // turn complex number into phase and gain
        for (int i = 0; i < num_bin; ++i) {
            auto cpx = std::complex{ real[i], imag[i] };
            if (i == 0)
                temp_gains[i] = std::abs(cpx) / analyze_fft_size;
            else
                temp_gains[i] = std::abs(cpx) / (analyze_fft_size / 2);
            curr_phases[i] = std::arg(cpx);
        }

        ResynthsisFrames::FftFrame new_frame;
        if (audio_frames.frames.empty()) {
            // startup frame
            // use 抛物线 to find the best freq and gain partial

            // https://cloud.tencent.com/developer/ask/sof/105773
            //auto find_parabola_peak = [](float x1, float y1, float x2, float y2, float x3, float y3)
            //{
            //    long double denom = (x1 - x2) * (x1 - x3) * (x2 - x3);
            //    long double A = (x3 * (y2 - y1) + x2 * (y1 - y3) + x1 * (y3 - y2)) / denom;
            //    long double B = (x3 * x3 * (y1 - y2) + x2 * x2 * (y3 - y1) + x1 * x1 * (y2 - y3)) / denom;
            //    long double C = (x2 * x3 * (x2 - x3) * y1 + x3 * x1 * (x3 - x1) * y2 + x1 * x2 * (x1 - x2) * y3) / denom;

            //    struct R {
            //        float xv;
            //        float yv;
            //    } r{};
            //    long double inv_A = 1.0 / A;
            //    long double xx = -B * 0.5 * inv_A;
            //    long double yy = C - B * B * 0.25 * inv_A;
            //    r.xv = xx;
            //    r.yv = yy;
            //    return r;
            //};
            // https://ccrma.stanford.edu/~jos/sasp/Quadratic_Interpolation_Spectral_Peaks.html
            auto find_parabola_peak = [](double x1, double y1, double x2, double y2, double x3, double y3) {
                struct R {
                    float xv;
                    float yv;
                } r{};

                double alpha = y1;
                double beta = y2;
                double gama = y3;
                auto up = alpha - gama;
                auto down = alpha - 2.0 * beta + gama;
                auto p = 0.5 * up / down;
                r.xv = x2 + p * (x2 - x1);
                r.yv = beta - 0.25 * p * (alpha - gama);
                return r;
            };

            // convert to db scale
            std::vector<float> temp_db_gains(num_bin);
            for (int i = 0; auto gain : temp_gains) {
                temp_db_gains[i++] = utli::GainToDb(gain);
            }

            // shit parabola only have num_bin - 2 data
            struct GainAndFreqPhase {
                float freq;
                float db_gain;
                float phase;
            };
            std::vector<GainAndFreqPhase> parabola_datas(num_bin - 1);
            for (int i = 0; i < num_bin - 2; ++i) {
                auto left_freq = i * source_sample_rate / analyze_fft_size;
                auto center_freq = (i + 1) * source_sample_rate / analyze_fft_size;
                auto right_freq = (i + 2) * source_sample_rate / analyze_fft_size;
                if (temp_db_gains[i] < temp_db_gains[i + 1]
                    && temp_db_gains[i + 1] > temp_db_gains[i + 2]) {
                    auto [peak_freq, peak_db] = find_parabola_peak(left_freq, temp_db_gains[i],
                                                                   center_freq, temp_db_gains[i + 1],
                                                                   right_freq, temp_db_gains[i + 2]);
                    parabola_datas[i].freq = peak_freq;
                    parabola_datas[i].db_gain = peak_db;

                    if (peak_freq < center_freq) { // interp phase with left and center
                        auto nor_x = (peak_freq - left_freq) / (center_freq - left_freq);
                        auto interp_cpx = nor_x * std::complex{ real[i], imag[i] } + (1.0f - nor_x) * std::complex{ real[i + 1],imag[i + 1] };
                        parabola_datas[i].phase = std::arg(interp_cpx);
                    }
                    else if (peak_freq > center_freq) { // interp phase with right and center
                        auto nor_x = (peak_freq - center_freq) / (right_freq - center_freq);
                        auto interp_cpx = nor_x * std::complex{ real[i + 1], imag[i + 1] } + (1.0f - nor_x) * std::complex{ real[i + 2],imag[i + 2] };
                        parabola_datas[i].phase = std::arg(interp_cpx);
                    }
                    else { // equal to center phase
                        parabola_datas[i].phase = curr_phases[i + 1];
                    }
                }
                else {
                    parabola_datas[i].freq = center_freq;
                    parabola_datas[i].db_gain = temp_db_gains[i + 1];
                    parabola_datas[i].phase = curr_phases[i + 1];
                }
            }
            // copy the last bin
            auto& last_parabola_data = parabola_datas.back();
            last_parabola_data.db_gain = temp_db_gains.back();
            last_parabola_data.freq = (num_bin - 1) * source_sample_rate / analyze_fft_size;
            last_parabola_data.phase = curr_phases.back();
            //{ // mirror freqs
            //    const auto left_idx = num_bin - 2;
            //    const auto center_idx = num_bin - 1;
            //    const auto right_idx = left_idx;
            //    auto left_freq = (num_bin - 2) * source_sample_rate / analyze_fft_size;
            //    auto center_freq = (num_bin - 1) * source_sample_rate / analyze_fft_size;
            //    auto right_freq = (num_bin)*source_sample_rate / analyze_fft_size;
            //    auto [peak_freq, peak_db] = find_parabola_peak(left_freq, temp_db_gains[left_idx],
            //                                                   center_freq, temp_db_gains[center_idx],
            //                                                   right_freq, temp_db_gains[num_bin - 2]);
            //    parabola_datas.back().freq = peak_freq;
            //    parabola_datas.back().db_gain = peak_db;

            //    if (peak_freq < center_freq) { // interp phase with left and center
            //        auto nor_x = (peak_freq - left_freq) / (center_freq - left_freq);
            //        auto interp_cpx = nor_x * std::complex{ real[left_freq], imag[left_idx] } +
            //            (1.0f - nor_x) * std::complex{ real[center_idx],imag[center_idx] };
            //        parabola_datas.back().phase = std::arg(interp_cpx);
            //    }
            //    else if (peak_freq > center_freq) { // interp phase with right and center
            //        auto nor_x = (peak_freq - center_freq) / (right_freq - center_freq);
            //        auto interp_cpx = nor_x * std::complex{ real[center_idx], imag[center_idx] }
            //        + (1.0f - nor_x) * std::complex{ real[right_idx],imag[right_idx] };
            //        parabola_datas.back().phase = std::arg(interp_cpx);
            //    }
            //    else { // equal to center phase
            //        parabola_datas.back().phase = curr_phases[center_idx];
            //    }
            //}
            // copy phase for next frame
            for (int i = 0; auto & p : last_frame_phases) {
                p = parabola_datas[i++].phase;
            }
            // trying to find the best partial
            for (int i = 0; i < kNumPartials; ++i) {
                auto res_partial_freq = c2_freq * (i + 1.0f);
                auto min_freq = res_partial_freq - max_search_freq_diff;
                auto max_freq = res_partial_freq + max_search_freq_diff;
                auto begin_it = std::ranges::find_if(parabola_datas, [min_freq](auto d) {return d.freq >= min_freq; });
                auto end_it = std::ranges::find_if(parabola_datas, [max_freq](auto d) {return d.freq > max_freq; });

                auto max_gain_it = std::max_element(begin_it, end_it, [](GainAndFreqPhase max, GainAndFreqPhase ele) {
                    return ele.db_gain > max.db_gain;
                });
                auto max_gain = *max_gain_it;

                auto ratio_diff = (max_gain.freq - res_partial_freq) / c2_freq;
                auto gain = utli::DbToGain(max_gain.db_gain);
                new_frame.gains[i] = gain;
                new_frame.ratio_diffs[i] = ratio_diff;
            }
            /*const auto c = std::complex(real[i + 1], imag[i + 1]);
            new_frame.gains[i] = std::abs(c) / (analyze_fft_size / 2);
            new_frame.freq_diffs[i] = 0.0f;
            new_frame.ratio_diffs[i] = 0.0f;
            curr_phases[i] = std::arg(c);*/
        }
        else { // todo: use phase lock
            struct GainAndFreq {
                float freq;
                float gain;
            };
            std::vector<GainAndFreq> high_resolution_infos(num_bin - 1);
            for (int i = 1; i < num_bin; ++i) {
                // calculate instant frequency
                const auto bin_frequency = static_cast<float>(i) * std::numbers::pi_v<float> *2.0f / static_cast<float>(analyze_fft_size);
                const auto target_phase = bin_frequency * kFFtHop + last_frame_phases[i - 1];
                const auto phase_diff = PhaseWrap(curr_phases[i] - target_phase);
                const auto nor_instant_freq = phase_diff / (kFFtHop * std::numbers::pi_v<float> *2.0f) + static_cast<float>(i) / static_cast<float>(analyze_fft_size);
                high_resolution_infos[i - 1].freq = nor_instant_freq * source_sample_rate;
                high_resolution_infos[i - 1].gain = temp_gains[i];
                last_frame_phases[i - 1] = curr_phases[i];
            }

            // trying to find the best partial
            for (int i = 0; i < kNumPartials; ++i) {
                auto res_partial_freq = c2_freq * (i + 1.0f);
                auto min_freq = res_partial_freq - max_search_freq_diff;
                auto max_freq = res_partial_freq + max_search_freq_diff;
                auto begin_it = std::ranges::find_if(high_resolution_infos, [min_freq](auto d) {return d.freq >= min_freq; });
                auto end_it = std::ranges::find_if(high_resolution_infos, [max_freq](auto d) {return d.freq > max_freq; });

                auto max_gain_it = std::max_element(begin_it, end_it, [](GainAndFreq max, GainAndFreq ele) {
                    return ele.gain > max.gain;
                });
                auto max_gain = *max_gain_it;

                auto ratio_diff = (max_gain.freq - res_partial_freq) / c2_freq;
                new_frame.gains[i] = max_gain.gain;
                new_frame.ratio_diffs[i] = ratio_diff;
            }

            //for (int i = 0; i < analyze_fft_size / 2; ++i) {
            //    const auto c = std::complex(real[i + 1], imag[i + 1]);
            //    new_frame.gains[i] = std::abs(c) / (analyze_fft_size / 2);
            //    auto this_frame_phase = std::arg(c);

            //    // calculate instant frequency
            //    const auto bin_frequency = static_cast<float>(i + 1) * std::numbers::pi_v<float> *2.0f / static_cast<float>(analyze_fft_size);
            //    const auto target_phase = bin_frequency * kFFtHop + curr_phases[i];
            //    const auto phase_diff = PhaseWrap(this_frame_phase - target_phase);
            //    const auto instant_freq = phase_diff / (kFFtHop * std::numbers::pi_v<float>) + (1.0f + i) / static_cast<float>(analyze_fft_size / 2);
            //    new_frame.ratio_diffs[i] = instant_freq * sample_rate_ratio / c2_freq - (i + 1.0f);
            //    curr_phases[i] = this_frame_phase;
            //}
        }
        max_gain = std::max(max_gain, *std::ranges::max_element(new_frame.gains));
        audio_frames.frames.emplace_back(std::move(new_frame));
        read_pos += kFFtHop;
    }

    if (max_gain == 0.0f) {
        auto gain_level_up = 1.0f / max_gain; // maybe 0.0f?
        for (auto& frame : audio_frames.frames) {
            for (auto& level : frame.gains) {
                level *= gain_level_up;
            }
        }
    }

    return audio_frames;
}

ResynthsisFrames Synth::CreateResynthsisFramesFromImage(std::unique_ptr<ImageBase> image_in) {
    ResynthsisFrames image_frame;

    /*
    * R nothing
    * G gain       [0,255] map to [-60,0]dB
    * B ratio_diff [0,255] map to [-1, 1]
    * simulate harmor's audio convert to image resynthsis mode
    */
    auto w = image_in->GetWidth();
    auto h = image_in->GetHeight();
    image_frame.frames.resize(w);
    auto max_gain = 0.0f;
    image_frame.frame_interval_sample = kFFtHop;
    const auto c2_freq = std::exp2(36.0f / 12.0f) * 8.1758f * 2.0f / sample_rate_;
    image_frame.base_freq = c2_freq;

    for (int x = 0; x < w; ++x) {
        auto& frame = image_frame.frames[x];
        for (int y = 0; y < kNumPartials; ++y) {
            auto y_nor = static_cast<float>(y) / static_cast<float>(kNumPartials);
            auto image_y_idx = std::clamp(static_cast<int>(h - y_nor * h), 0, h - 1);
            auto pixel = image_in->GetPixel(x, image_y_idx);

            // map g to gain
            auto gain = 0.0f;
            if (pixel.g != 0) {
                auto db = std::lerp(-60.0f, 0.0f, static_cast<float>(pixel.g) / 255.0f);
                gain = Db2Gain(db) / (y + 1.0f);
            }
            max_gain = std::max(gain, max_gain);

            // map b to ratio diff
            auto ratio_diff = 2.0f * static_cast<float>(pixel.b) / 255.0f - 1.0f;
            frame.gains[y] = gain;
            frame.ratio_diffs[y] = ratio_diff;
        }
        //for (int y = kNumPartials; y < kFFtSize / 2; ++y) {
        //    frame.gains[y] = 0.0f;
        //    frame.ratio_diffs[y] = 0.0f;
        //}
    }

    auto gain_level_up = 1.0f / max_gain; // maybe 0.0f?
    for (auto& frame : image_frame.frames) {
        for (auto& level : frame.gains) {
            level *= gain_level_up;
        }
    }

    return image_frame;
}
}