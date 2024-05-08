#pragma once

#include <random>
#include <ranges>
#include <algorithm>
#include "engine/IProcessor.h"
#include "param/effect_param.h"
#include "param/param.h"
#include "utli/convert.h"

namespace mana {
class Decay : public IProcessor {
public:
    void Init(float sample_rate, float update_rate) override {
        sample_rate_ = sample_rate;
    }

    void Process(Partials& partials) override {
        for (int i = 0; i < kNumPartials; ++i) {
            partials.gains[i] *= gains_[i];

            auto damp_val = utli::DbToGain(std::min(0.0f, (partials.pitches[i] - partials.base_pitch) / 120.0f * decay_slope_));
            auto d = damp_val * decay_a_;

            gains_[i] *= d;
        }
    }

    void OnUpdateTick(const SynthParam& params, int skip, int module_idx) override {
        auto update_rate = sample_rate_ / static_cast<float>(skip);

        decay_slope_ = param::Decay_Slope::GetNumber(params.effects[module_idx].args);
        decay_time_ = param::Decay_Time::GetNumber(params.effects[module_idx].args);

        constexpr auto kSilenceDb = -60.0f;
        decay_a_ = utli::Calc1stSmoothFilterCoeffByDecayRate((-kSilenceDb * 1000.0f) / decay_time_, update_rate);
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
    std::array<float, kNumPartials> gains_{};
};
}