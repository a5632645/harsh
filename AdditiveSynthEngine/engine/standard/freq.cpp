#include "freq.h"

#include <cmath>
#include "param/standard_param.h"

namespace mana {
void FreqProcessor::Init(float sample_rate) {
    reciprocal_nyquist_rate_ = 2.0f / sample_rate;
}

void FreqProcessor::OnUpdateTick(const SynthParam& param, int skip) {
    base_pitch_ = note_pitch_ + param::FloatParam<param::PitchBend>::GetNumber(param.standard.pitch_bend);
    base_frequency_ = std::exp2(base_pitch_ / 12.0f) * 8.1758f * reciprocal_nyquist_rate_;
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
    partials.base_pitch = base_pitch_;
}
}