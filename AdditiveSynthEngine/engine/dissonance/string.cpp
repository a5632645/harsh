#include "string.h"

#include <numbers>
#include <cmath>
#include "param/dissonance_param.h"

static constexpr float kRatios[]{ 1.0f, 0.1f,0.01f,0.001f,0.0001f };

namespace mana {
void StringDissonance::Init(float sample_rate) {
}

void StringDissonance::Process(Partials& partials) {
    for (int i = 0; i < kNumPartials; ++i) {
        auto n = i + 1;
        auto ratio = n * std::sqrt(1.0f + n * n * stretch_factor_);
        partials.freqs[i] = partials.base_frequency * ratio;
        partials.pitches[i] = partials.base_pitch + 12.0f * std::log2(ratio);
    }
}

void StringDissonance::OnUpdateTick(const SynthParam& param, int skip) {
    auto ratio_idx = param::FloatChoiceParam<param::StringMultiRatio, param::StringMultiRatio::RatioEnum>::GetChoiceIndex(
        param.dissonance.arg1
    );
    ratio_multi_ = kRatios[ratio_idx];
    stretch_factor_ = param::FloatParam<param::StringDissStretch>::GetNumber(param.dissonance.arg0) * ratio_multi_;
}

void StringDissonance::OnNoteOn(int note) {
}

void StringDissonance::OnNoteOff() {
}
}