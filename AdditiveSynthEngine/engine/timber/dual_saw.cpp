#include "dual_saw.h"

#include <numbers>
#include <cmath>
#include "engine/VolumeTable.hpp"
#include <engine/EngineConfig.h>

namespace mana {

static constexpr auto kSawHarmonicGains = VolumeTable<kNumPartials>::SAW_TABLE;
static constexpr auto kTwoPi = std::numbers::pi_v<float> *2.0f;

DualSaw::DualSaw(
    const param::DualSawParam & param
) : synth_param_(param) {
}

void DualSaw::Init(
    float sample_rate
) {
    sample_rate_ = sample_rate;
}

void DualSaw::Process(
    Partials& partials
) {
    auto sign = synth_param_.beating_rate_ >= 0.0f ? 0.5f : -0.5f;
    auto second_ratio = static_cast<int>(synth_param_.ratio_);

    if (second_ratio != 1) {
        for (int i = 0; i < kNumPartials; ++i) {
            // odd
            partials.gains[i] = kSawHarmonicGains[i];

            ++i;
            // even
            partials.gains[i] = kSawHarmonicGains[i] * synth_param_.saw_square_;
        }
    }

    int second_partial_idx = second_ratio - 1;
    for (int i = 0; second_partial_idx < kNumPartials; ++i) {
        auto gain = kSawHarmonicGains[i];
        if ((i & 1) == 1) {
            gain = kSawHarmonicGains[i] * synth_param_.saw_square_;
        }

        auto beating_gain = 0.5f + sign * std::cos((i + 1.0f) * kTwoPi * beating_phase_);
        partials.gains[second_partial_idx] = beating_gain * gain;
        second_partial_idx += second_ratio;
    }

    
}

void DualSaw::OnUpdateTick(
    int skip
) {
    auto inc = std::abs(synth_param_.beating_rate_) * skip / sample_rate_;
    beating_phase_ += inc;
    if (beating_phase_ > 1.0f) {
        beating_phase_ -= 1.0f;
    }
    if (beating_phase_ < 0.0f) {
        beating_phase_ += 1.0f;
    }
}

void DualSaw::OnNoteOn(int note) {
    beating_phase_ = 0.0f;
}

void DualSaw::OnNoteOff() {
}

}