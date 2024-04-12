#include "freq.h"

#include <cmath>

namespace mana {
FreqProcessor::FreqProcessor(const param::VoiceParam & param)
    : voice_param_(param) {
}

void FreqProcessor::Init(float sample_rate) {
    reciprocal_nyquist_rate_ = 2.0f / sample_rate;
}

void FreqProcessor::OnUpdateTick(int skip) {
    auto pitch = note_pitch_ + voice_param_.pitch_bend;
    base_frequency_ = std::exp2(pitch / 12.0f) * 8.1758f * reciprocal_nyquist_rate_;
}

void FreqProcessor::OnNoteOn(int note) {
    note_pitch_ = note;
}

void FreqProcessor::OnNoteOff() {
    note_pitch_ = -1;
}

void FreqProcessor::Process(Partials& partials) {
    float freq = base_frequency_;

    partials.base_frequency = base_frequency_;
    for (size_t i = 0; i < kNumPartials; ++i) {
        partials.freqs[i] = freq;
        freq += base_frequency_;
    }
}
}