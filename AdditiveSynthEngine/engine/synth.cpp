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

ResynthsisFrames Synth::CreateResynthsisFrames(const std::vector<float>& sample, float sample_rate) {
    audiofft::AudioFFT fft;
    Window<float> window;
    fft.init(kFFtSize);
    window.Init(kFFtSize);

    const auto sample_rate_ratio = sample_rate / sample_rate_;
    const auto c2_freq = std::exp2(36.0f / 12.0f) * 8.1758f * 2.0f / sample_rate_;
    auto num_frame = static_cast<size_t>((sample.size() - static_cast<float>(kFFtSize)) / static_cast<float>(kFFtHop));
    ResynthsisFrames audio_frames;
    audio_frames.frames.reserve(num_frame);

    auto read_pos = 0;
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
            for (int i = 0; i < kNumPartials; ++i) {
                const auto c = std::complex(real[i + 1], imag[i + 1]);
                new_frame.gains[i] = std::abs(c) / kHarmonics;
                new_frame.freq_diffs[i] = 0.0f;
                phases[i] = std::arg(c);
            }
        }
        else {
            const auto& last_frame = audio_frames.frames.back();
            for (int i = 0; i < kNumPartials; ++i) {
                const auto c = std::complex(real[i + 1], imag[i + 1]);
                new_frame.gains[i] = std::abs(c) / kHarmonics;
                auto this_frame_phase = std::arg(c);

                // calculate instant frequency
                const auto bin_frequency = static_cast<float>(i + 1) * std::numbers::pi_v<float> *2.0f / static_cast<float>(kFFtSize);
                const auto target_phase = bin_frequency * kFFtHop + phases[i];
                const auto phase_diff = PhaseWrap(this_frame_phase - target_phase);
                const auto instant_freq = phase_diff * 2.0f / (kFFtHop * std::numbers::pi_v<float>) + (1.0f + i) / static_cast<float>(kFFtSize / 2);
                new_frame.freq_diffs[i] = instant_freq * sample_rate_ratio - c2_freq * (1.0f + i);
                phases[i] = this_frame_phase;
            }
        }
        audio_frames.frames.emplace_back(std::move(new_frame));
        read_pos += kFFtHop;
    }

    audio_frames.data_sample_rate = sample_rate;
    audio_frames.data_series_freq = c2_freq;
    return audio_frames;
}
}