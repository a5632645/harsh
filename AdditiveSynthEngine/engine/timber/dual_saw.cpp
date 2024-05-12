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

void DualSaw::Init(float sample_rate, float update_rate) {
    sample_rate_ = sample_rate;
    inv_update_rate_ = 1.0f / update_rate;
}

void DualSaw::Process(TimberFrame& frame) {
    auto second_ratio = static_cast<int>(ratio_);

    for (int i = 0; i < kNumPartials; ++i) {
        // odd
        frame.gains[i] = kSawHarmonicGains[i];

        ++i;
        // even
        frame.gains[i] = kSawHarmonicGains[i] * saw_square_;
    }

    int proc_idx = 0;
    for (int i = ratio_ - 1; i < kNumPartials; i += ratio_) {
        auto gain = kSawHarmonicGains[proc_idx] * second_amp_;
        if ((proc_idx & 1) == 1) {
            gain *= saw_square_;
        }

        auto fade = std::cos(std::numbers::pi_v<float> *2.0f * (proc_idx + 1.0f) * beating_phase_);
        auto fade01 = 0.5f + 0.5f * fade;
        frame.gains[i] = std::lerp(frame.gains[i] - gain,
                                   frame.gains[i] + gain,
                                   fade01);
        ++proc_idx;
    }
}

void DualSaw::OnUpdateTick(OscParam& params) {
    ratio_ = param::DualSaw_Ratio::GetNumber(params.args);
    beating_rate_ = param::DualSaw_BeatingRate::GetNumber(params.args);
    saw_square_ = param::DualSaw_SawSquare::GetNumber(params.args);
    second_amp_ = param::DualSaw_SecondAmp::GetNumber(params.args);

    auto inc = beating_rate_ * inv_update_rate_;
    beating_phase_ += inc;
    beating_phase_ -= static_cast<float>(static_cast<int>(beating_phase_));
}

void DualSaw::OnNoteOn(int note) {
    if (beating_rate_ == 0.0f) {
        beating_phase_ = 0.0f;
    }
}

void DualSaw::OnNoteOff() {
}
}