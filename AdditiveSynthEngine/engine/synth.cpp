#include "synth.h"

namespace mana {
Synth::Synth() {
    m_oscillators.resize(kNumOscillors);
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

    std::ranges::transform(audio_buffer_, audio_buffer_.begin(), [](float v) {return v * 0.2f; });
}

void Synth::Init(size_t bufferSize, float sampleRate) {
    sample_rate_ = sampleRate;

    audio_buffer_.resize(bufferSize);

    for (Oscillor& o : m_oscillators) {
        o.Init(bufferSize, sampleRate);
    }
}

void Synth::update_state(int step) {
    param_bank_.UpdateParamOutput();
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
}