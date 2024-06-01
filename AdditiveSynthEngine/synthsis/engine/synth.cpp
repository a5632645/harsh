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

ResynthsisFrames Synth::CreateResynthsisFramesFromAudio(const std::vector<float>& sample, float sample_rate) {
    audiofft::AudioFFT fft;
    Window<float> window;
    fft.init(kFFtSize);
    window.Init(kFFtSize);

    const auto sample_rate_ratio = sample_rate / sample_rate_;
    const auto c2_freq = std::exp2(36.0f / 12.0f) * 8.1758f * 2.0f / sample_rate_;
    auto num_frame = static_cast<size_t>((sample.size() - static_cast<float>(kFFtSize)) / static_cast<float>(kFFtHop));
    ResynthsisFrames audio_frames;
    audio_frames.frames.reserve(num_frame);
    audio_frames.frame_interval_sample = kFFtHop / sample_rate_ratio;
    audio_frames.base_freq = c2_freq;

    auto read_pos = 0;
    auto max_gain = 0.0f;
    const auto num_bin = audiofft::AudioFFT::ComplexSize(kFFtSize);
    std::vector<float> phases(num_bin);
    std::vector<float> real(num_bin);
    std::vector<float> imag(num_bin);
    std::vector<float> fft_buffer(kFFtSize, 0.0f);
    while (num_frame--) {
        if (read_pos + kFFtSize <= sample.size()) {
            // directly fft from sample
            auto it = sample.begin() + read_pos;
            std::copy(it, it + kFFtSize, fft_buffer.begin());
        }
        else {
            // fill zero
            const auto num_samples_can_read = sample.size() - read_pos;
            for (int i = 0; i < num_samples_can_read; ++i) {
                fft_buffer[i] = sample.at(read_pos + i);
            }
        }

        window.ApplyWindow(fft_buffer);
        fft.fft(fft_buffer.data(), real.data(), imag.data());

        ResynthsisFrames::FftFrame new_frame;
        if (audio_frames.frames.empty()) {
            for (int i = 0; i < kFFtSize / 2; ++i) {
                const auto c = std::complex(real[i + 1], imag[i + 1]);
                new_frame.gains[i] = std::abs(c) / (kFFtSize / 2);
                new_frame.freq_diffs[i] = 0.0f;
                new_frame.ratio_diffs[i] = 0.0f;
                phases[i] = std::arg(c);
            }
        }
        else {
            const auto& last_frame = audio_frames.frames.back();
            for (int i = 0; i < kFFtSize / 2; ++i) {
                const auto c = std::complex(real[i + 1], imag[i + 1]);
                new_frame.gains[i] = std::abs(c) / (kFFtSize / 2);
                auto this_frame_phase = std::arg(c);

                // calculate instant frequency
                const auto bin_frequency = static_cast<float>(i + 1) * std::numbers::pi_v<float> *2.0f / static_cast<float>(kFFtSize);
                const auto target_phase = bin_frequency * kFFtHop + phases[i];
                const auto phase_diff = PhaseWrap(this_frame_phase - target_phase);
                const auto instant_freq = phase_diff / (kFFtHop * std::numbers::pi_v<float>) + (1.0f + i) / static_cast<float>(kFFtSize / 2);
                new_frame.ratio_diffs[i] = instant_freq * sample_rate_ratio / c2_freq - (i + 1.0f);
                phases[i] = this_frame_phase;
            }
        }
        max_gain = std::max(max_gain, *std::ranges::max_element(new_frame.gains));
        audio_frames.frames.emplace_back(std::move(new_frame));
        read_pos += kFFtHop;
    }

    auto gain_level_up = 1.0f / max_gain; // maybe 0.0f?
    for (auto& frame : audio_frames.frames) {
        for (auto& level : frame.gains) {
            level *= gain_level_up;
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