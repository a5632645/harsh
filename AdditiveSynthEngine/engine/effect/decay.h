#pragma once

#include <random>
#include <ranges>
#include <algorithm>
#include "effect_base.h"
#include "param/effect_param.h"
#include "utli/convert.h"

namespace mana {
class Decay : public EffectBase {
public:
    void Init(float sample_rate, float update_rate) override {
        sample_rate_ = sample_rate;
        update_rate_ = update_rate;
    }

    void Process(Partials& partials) override {
        for (int i = 0; i < kNumPartials; ++i) {
            partials.gains[i] *= gains_[i];

            auto damp_val = utli::DbToGain(std::min(0.0f, (partials.pitches[i] - partials.base_pitch) / 120.0f * decay_slope_));
            auto d = damp_val * decay_a_;

            gains_[i] *= d;
        }
    }

    void OnUpdateTick(EffectParams& args, CurveManager& curves) override {
        decay_slope_ = param::Decay_Slope::GetNumber(args.args);
        decay_time_ = param::Decay_Time::GetNumber(args.args);

        constexpr auto kSilenceDb = -60.0f;
        decay_a_ = utli::Calc1stSmoothFilterCoeffByDecayRate((-kSilenceDb * 1000.0f) / decay_time_, update_rate_);
    }

    void OnNoteOn(int note) override {
        gains_.fill(1.0f);
    }

    void OnNoteOff() override {}
private:
    float decay_time_{};
    float decay_slope_{};
    float decay_a_{};
    float sample_rate_{};
    float update_rate_{};
    std::array<float, kNumPartials> gains_{};
};
}