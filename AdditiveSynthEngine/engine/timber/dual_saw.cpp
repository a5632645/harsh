#include "dual_saw.h"

#include <numbers>
#include <cmath>
#include "engine/VolumeTable.hpp"
#include <engine/EngineConfig.h>
#include "param/timber_param.h"
#include "param/param.h"

namespace mana {
static constexpr auto kSawHarmonicGains = makeHarmonicArray<kNumPartials>([](float v) {
    return  1.0f / v / std::numbers::pi_v<float>;
});
static constexpr auto kTwoPi = std::numbers::pi_v<float> *2.0f;

void DualSaw::Init(float sample_rate) {
    sample_rate_ = sample_rate;
}

void DualSaw::Process(Partials& partials) {
    auto second_ratio = static_cast<int>(ratio_);

    for (int i = 0; i < kNumPartials; ++i) {
        // odd
        partials.gains[i] = kSawHarmonicGains[i];

        ++i;
        // even
        partials.gains[i] = kSawHarmonicGains[i] * saw_square_;
    }

    int second_partial_idx = second_ratio - 1;
    for (int i = 0; second_partial_idx < kNumPartials; ++i) {
        auto gain = kSawHarmonicGains[i];
        if ((i & 1) == 1) {
            gain = kSawHarmonicGains[i] * saw_square_;
        }

        auto beating_gain = 0.5f + 0.5f * std::cos((i + 1.0f) * kTwoPi * beating_phase_);
        partials.gains[second_partial_idx] += gain;
        partials.gains[second_partial_idx] *= beating_gain;
        second_partial_idx += second_ratio;
    }
}

void DualSaw::OnUpdateTick(const SynthParam& param, int skip, int module_idx) {
    ratio_ = param::DualSaw_Ratio::GetNumber(param.timber.args);
    beating_rate_ = param::DualSaw_BeatingRate::GetNumber(param.timber.args);
    saw_square_ = param::DualSaw_SawSquare::GetNumber(param.timber.args);

    auto inc = beating_rate_ * skip / sample_rate_;
    beating_phase_ += inc;
    beating_phase_ -= static_cast<float>(static_cast<int>(beating_phase_));
}

void DualSaw::OnNoteOn(int note) {
    beating_phase_ = 0.0f;
}

void DualSaw::OnNoteOff() {
}
}