#pragma once

#include <numbers>
#include <random>
#include <complex>
#include "engine/IProcessor.h"
#include "param/effect_param.h"
#include "param/param.h"

namespace mana {
class Blur : public IProcessor {
public:
    void Init(float sample_rate, float update_rate) override {
        sample_rate_ = sample_rate;
    }

    void Process(Partials& partials) override {
        float bin_old_val = partials.gains.front();

        for (int i = 0; i < kNumPartials; ++i) {
            bin_old_val = ExpSmoothFilter(bin_att_param_, bin_rel_param_, bin_old_val, partials.gains[i]);
            partials.gains[i] = bin_old_val;
        }

        for (int i = 0; i < kNumPartials; ++i) {
            auto v = ExpSmoothFilter(time_att_param_, time_rel_param_, latched_gains_[i], partials.gains[i]);
            latched_gains_[i] = v;
            partials.gains[i] = v;
        }
    }

    void OnUpdateTick(const SynthParam& params, int skip, int module_idx) override {
        auto update_rate = sample_rate_ / static_cast<float>(skip);

        time_att_param_ = Calc1stSmoothFilterCoeff(param::Blur_TimeAttack::GetNumber(params.effects[module_idx].args), update_rate);
        time_rel_param_ = Calc1stSmoothFilterCoeff(param::Blur_TimeRelease::GetNumber(params.effects[module_idx].args), update_rate);
        bin_att_param_ = param::Blur_BinAttack::GetNumber(params.effects[module_idx].args);
        bin_rel_param_ = param::Blur_BinRelease::GetNumber(params.effects[module_idx].args);
    }
    void OnNoteOn(int note) override {}
    void OnNoteOff() override {}
private:
    inline static float Calc1stSmoothFilterCoeff(float time_milliseconds, float system_rate) {
        if (time_milliseconds < 1.0f) {
            return 0.0f;
        }
        return std::exp(-1.0f / (system_rate * time_milliseconds * 0.001f));
    }

    inline static float ExpSmoothFilter(float att, float rel, float old, float new_val) {
        if (new_val > old) {
            return att * old + (1.0f - att) * new_val;
        }
        else {
            return rel * old + (1.0f - rel) * new_val;
        }
    }

    float sample_rate_{};
    std::array<float, kNumPartials> latched_gains_{};
    float time_att_param_{};
    float time_rel_param_{};
    float bin_att_param_{};
    float bin_rel_param_{};
};
}