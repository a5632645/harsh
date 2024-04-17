#include "string.h"

#include <numbers>
#include <cmath>
#include "param/dissonance_param.h"

namespace mana {
void StringDissonance::Init(float sample_rate) {
}

void StringDissonance::Process(Partials& partials) {
    for (int i = 0; i < kNumPartials; ++i) {
        auto n = i + 1;
        auto ratio = n * std::sqrt(1.0f + n * n * stretch_factor_);
        partials.freqs[i] = partials.base_frequency * ratio;
    }
}

void StringDissonance::OnUpdateTick(const SynthParam& param, int skip) {
    stretch_factor_ = param::FloatParam<param::StringDissStretch>::GetNumber(param.dissonance.arg0);
}

void StringDissonance::OnNoteOn(int note) {
}

void StringDissonance::OnNoteOff() {
}
}