#include "freq.h"

#include "engine/oscillor_param.h"
#include "param/standard_param.h"
#include "utli/convert.h"

namespace mana {
void FreqProcessor::Init(float sample_rate, float update_rate) {
}

void FreqProcessor::OnUpdateTick() {
    base_pitch_ = note_pitch_ + pitch_bend_->GetValue();
    base_frequency_ = utli::PitchToFreq(base_pitch_);
}

void FreqProcessor::OnNoteOn(int note) {
    note_pitch_ = static_cast<float>(note);
}

void FreqProcessor::OnNoteOff() {
}

void FreqProcessor::PrepareParams(OscillorParams & params) {
    pitch_bend_ = params.GetPolyFloatParam("pitch_bend");
}

void FreqProcessor::Process(Partials& partials) {
    partials.base_frequency = base_frequency_;
    partials.base_pitch = base_pitch_;
}
}