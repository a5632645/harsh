#include "synth.h"

#include "resynthsis/window.h"
#include "param/standard_param.h"
#include "AudioFFT.h"

namespace mana {
Synth::Synth() {
    for (int i = 0; i < kNumOscillors; ++i) {
        m_oscillators.emplace_back(*this);
    }
    BindParam();
}

void Synth::NoteOn(int note, float velocity) {
    // rr one time
    for (size_t i = 0; i < kNumOscillors; ++i) {
        if (m_oscillators[m_rrPosition].canPlayNote(note)) {
            m_oscillators[m_rrPosition].NoteOn(note, velocity);
            m_rrPosition = (m_rrPosition + 1) % kNumOscillors;
            return;
        }
        m_rrPosition = (m_rrPosition + 1) % kNumOscillors;
    }
    // if not found
    // stop rr osciilor
    m_oscillators.at(m_rrPosition).NoteOff();
    // set current note
    m_oscillators.at(m_rrPosition).NoteOn(note, velocity);
    m_rrPosition = (m_rrPosition + 1) % kNumOscillors;
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

    for (Oscillor& o : m_oscillators) {
        o.renderBuffer(numFrame);
        std::ranges::transform(audio_buffer_, o.getBuffer(), audio_buffer_.begin(), std::plus<float>());
    }

    std::ranges::transform(audio_buffer_, audio_buffer_.begin(), [this](float v) {return v * output_gain_; });
}

void Synth::Init(size_t bufferSize, float sampleRate) {
    sample_rate_ = sampleRate;

    audio_buffer_.resize(bufferSize);

    for (Oscillor& o : m_oscillators) {
        o.Init(bufferSize, sampleRate);
    }
}

static float Db2Gain(float db) {
    return std::exp(0.11512925464970228420089957273422f * db);
}

void Synth::update_state(int step) {
    param_bank_.UpdateParamOutput();

    output_gain_ = Db2Gain(param::OutputGain::GetNumber(synth_param_.standard.output_gain));

    for (Oscillor& o : m_oscillators) {
        o.update_state(synth_param_, step);
    }
}

const Oscillor& Synth::GetDisplayOscillor() const {
    auto idx = static_cast<int>(m_rrPosition) - 1;
    auto last_played = (idx + kNumOscillors) % kNumOscillors;
    for (size_t i = 0; i < kNumOscillors; ++i) {
        idx += kNumOscillors;
        idx %= kNumOscillors;
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

ResynthsisFrames Synth::CreateResynthsisFramesFromAudio(const std::vector<float>& sample, float sample_rate) {
    audiofft::AudioFFT fft;
    Window<float> window;
    fft.init(kFFtSize);
    window.Init(kFFtSize);

    const auto sample_rate_ratio = sample_rate_ / sample_rate;
    const auto c2_freq = std::exp2(36.0f / 12.0f) * 8.1758f * 2.0f / sample_rate_;
    auto num_frame = static_cast<size_t>((sample.size() - static_cast<float>(kFFtSize)) / static_cast<float>(kFFtHop));
    ResynthsisFrames audio_frames;
    audio_frames.frames.reserve(num_frame);
    audio_frames.frame_interval_sample = sample_rate_ratio * kFFtHop;
    audio_frames.max_fre_diff = 1.0f / kFFtHop;

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
            for (int i = 0; i < /*kNumPartials*/ kFFtSize / 2; ++i) {
                const auto c = std::complex(real[i + 1], imag[i + 1]);
                new_frame.gains[i] = std::abs(c) / (kFFtSize / 2);
                new_frame.freq_diffs[i] = 0.0f;
                phases[i] = std::arg(c);
            }
        }
        else {
            const auto& last_frame = audio_frames.frames.back();
            for (int i = 0; i < /*kNumPartials*/ kFFtSize / 2; ++i) {
                const auto c = std::complex(real[i + 1], imag[i + 1]);
                new_frame.gains[i] = std::abs(c) / (kFFtSize / 2);
                auto this_frame_phase = std::arg(c);

                // calculate instant frequency
                const auto bin_frequency = static_cast<float>(i + 1) * std::numbers::pi_v<float> *2.0f / static_cast<float>(kFFtSize);
                const auto target_phase = bin_frequency * kFFtHop + phases[i];
                const auto phase_diff = PhaseWrap(this_frame_phase - target_phase);
                const auto instant_freq = phase_diff /** 2.0f*/ / (kFFtHop * std::numbers::pi_v<float>) + (1.0f + i) / static_cast<float>(kFFtSize / 2);
                new_frame.freq_diffs[i] = instant_freq * sample_rate_ratio - c2_freq * (1.0f + i);
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

    audio_frames.data_sample_rate = sample_rate;
    audio_frames.data_series_freq = c2_freq;
    return audio_frames;
}

ResynthsisFrames Synth::CreateResynthsisFramesFromImage(const std::vector<std::vector<SimplePixel>>& image_in) {

    ResynthsisFrames image_frame;

    /*
    * R nothing
    * G gain
    * B fre_diff
    */
    auto w = image_in.size();
    auto h = static_cast<int>(image_in.front().size());
    image_frame.frames.resize(w);
    auto max_gain = 0.0f;
    image_frame.data_sample_rate = sample_rate_;
    const auto c3_freq = std::exp2(24.0f / 12.0f) * 8.1758f * 2.0f / sample_rate_;
    image_frame.data_series_freq = c3_freq;

    auto loop_val = kNumPartials;
    image_frame.frame_interval_sample = kFFtHop;
    /*if (h > kNumPartials) {
        loop_val = kFFtSize / 2;
        image_frame.frame_interval_sample = kNumPartials;
    }*/
    auto max_fre_diff = 1.0f / static_cast<float>(image_frame.frame_interval_sample);
    image_frame.max_fre_diff = max_fre_diff;

    for (int x = 0; x < w; ++x) {
        auto& frame = image_frame.frames[x];
        auto& img_line = image_in[x];
        for (int y = 0; y < loop_val; ++y) {
            auto y_nor = static_cast<float>(y) / static_cast<float>(loop_val);
            auto image_y_idx = std::clamp(static_cast<int>(h - y_nor * h), 0, h - 1);
            auto pixel = img_line[image_y_idx];

            auto gain = 0.0f;
            if (pixel.g != 0) {
                auto db = std::lerp(-60.0f, 0.0f, static_cast<float>(pixel.g) / 255.0f);
                gain = Db2Gain(db) / (y + 1.0f);
            }
            auto fre_diff_nor = 2.0f * static_cast<float>(pixel.b) / 255.0f - 1.0f;
            auto fre_diff = max_fre_diff * fre_diff_nor;
            //auto gain = static_cast<float>(pixel.g) / 255.0f;
            max_gain = std::max(gain, max_gain);

            frame.gains[y] = gain;
            frame.freq_diffs[y] = fre_diff;
        }
        for (int y = loop_val; y < kFFtSize / 2; ++y) {
            frame.gains[y] = 0.0f;
            frame.freq_diffs[y] = 0.0f;
        }
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