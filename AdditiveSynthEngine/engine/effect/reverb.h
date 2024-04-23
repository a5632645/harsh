#pragma once

#include <random>
#include <ranges>
#include <algorithm>
#include "engine/IProcessor.h"
#include "param/effect_param.h"
#include "param/param.h"

namespace mana {
class Reverb : public IProcessor {
public:
    // 通过 IProcessor 继承
    void Init(float sample_rate) override {
        sample_rate_ = sample_rate;
    }

    void Process(Partials& partials) override {
        for (int i = 0; i < kNumPartials; ++i) {
            auto abs_gain = std::abs(partials.gains[i]);

            // calc decay var
            auto decay = Db2Gain((30.0f - partials.pitches[i]) * damp_ * 0.01f) * decay_a_;
            decay = std::min(1.0f, decay);

            // do 1st smoothing
            if (abs_gain > latched_gains[i]) {
                latched_gains[i] = latched_gains[i] * attack_a_ + (1.0f - attack_a_) * abs_gain;
            }
            else {
                latched_gains[i] = latched_gains[i] * decay + (1.0f - decay) * abs_gain;
            }

            // noise

            auto noise_gain = urnd_(random_dev_);
            noise_gain = 2.0f * noise_gain - 1.0f;
            auto noise_smooth = last_noise_out_ + noise_gain;
            last_noise_out_ = noise_gain;
            partials.gains[i] += latched_gains[i] * noise_smooth * amount_;
        }
    }

    void OnUpdateTick(const SynthParam& params, int skip, int module_idx) override {
        system_rate_ = sample_rate_ / static_cast<float>(skip);

        amount_ = param::FloatParam<param::Reverb_Amount>::GetNumber(params.effects[module_idx].args);
        decay_ = param::FloatParam<param::Reverb_Decay>::GetNumber(params.effects[module_idx].args);
        attack_ = param::FloatParam<param::Reverb_Attack>::GetNumber(params.effects[module_idx].args);
        damp_ = param::FloatParam<param::Reverb_Damp>::GetNumber(params.effects[module_idx].args);

        attack_a_ = Calc1stSmoothFilterCoeff(attack_ / 1000.0f, system_rate_);
        decay_a_ = Calc1stSmoothFilterCoeff(decay_ / 1000.0f, system_rate_);
        if (decay_ == param::Reverb_Decay::kMax) {
            decay_a_ = 1.0f;
        }
    }
    void OnNoteOn(int note) override {}
    void OnNoteOff() override {}
private:
    static float Db2Gain(float db) {
        return std::exp(0.11512925464970228420089957273422f * db);
    }

    inline static float Calc1stSmoothFilterCoeff(float time_second, float system_rate) {
        if (time_second < 1.0f / 1000.0f) {
            return 0.0f;
        }
        return std::exp(-1.0f / (system_rate * time_second));
    }

    std::random_device random_dev_{};
    std::uniform_real_distribution<float> urnd_;
    std::array<float, kNumPartials> latched_gains{};
    float amount_{};
    float decay_{};
    float attack_{};
    float damp_{};
    float sample_rate_{};
    float attack_a_{};
    float decay_a_{};
    float system_rate_{};
    float last_noise_out_{};
};
}