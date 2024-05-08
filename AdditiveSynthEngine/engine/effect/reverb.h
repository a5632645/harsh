#pragma once

#include <random>
#include <ranges>
#include <algorithm>
#include "engine/IProcessor.h"
#include "param/effect_param.h"
#include "param/param.h"
#include "utli/convert.h"

namespace mana {
class Reverb : public IProcessor {
public:
    void Init(float sample_rate, float update_rate) override {
        sample_rate_ = sample_rate;
    }

    void Process(Partials& partials) override {
        for (int i = 0; i < kNumPartials; ++i) {
            auto abs_gain = std::abs(partials.gains[i]);

            // do 1st smoothing
            if (abs_gain > latched_gains_[i]) {
                latched_gains_[i] = latched_gains_[i] * attack_a_ + (1.0f - attack_a_) * abs_gain;
            }
            else {
                auto damp_val = utli::DbToGain(std::min(0.0f, damp_ * (60.0f - partials.pitches[i]) / 12.0f * 0.1f));
                auto d = damp_val * decay_a_;
                latched_gains_[i] = latched_gains_[i] * d + (1.0f - d) * abs_gain;
            }

            // noise
            auto f = partials.freqs[i] * sample_rate_ * 0.5f * dp_ff_ + 0.333f;
            auto inc = f * inv_system_rate_;
            noise_phase_[i] += inc;
            if (noise_phase_[i] > 1.0f) {
                auto noise_gain = urnd_(random_dev_);
                noise_gain = 2.0f * noise_gain - 1.0f;
                latched_noise2_[i] = latched_noise_[i];
                latched_noise_[i] = noise_gain;
                noise_phase_[i] -= static_cast<int>(noise_phase_[i]);
            }

            auto noise_smooth = std::lerp(latched_noise2_[i], latched_noise_[i], noise_phase_[i]);
            auto wet_val = latched_gains_[i] * noise_smooth * amount_;
            auto dry_mix = 1.0f - amount_;
            auto wet_mix = amount_;
            partials.gains[i] = dry_mix * partials.gains[i] + wet_mix * wet_val;
        }
    }

    void OnUpdateTick(const SynthParam& params, int skip, int module_idx) override {
        update_rate_ = sample_rate_ / static_cast<float>(skip);
        inv_system_rate_ = 1.0f / update_rate_;

        amount_ = param::Reverb_Amount::GetNumber(params.effects[module_idx].args);
        decay_time_ = param::Reverb_Decay::GetNumber(params.effects[module_idx].args);
        attack_ = param::Reverb_Attack::GetNumber(params.effects[module_idx].args);
        damp_ = param::Reverb_Damp::GetNumber(params.effects[module_idx].args);

        attack_a_ = utli::Calc1stSmoothFilterCoeff(attack_ / 1000.0f, update_rate_);
        constexpr auto kSilenceDb = -60.0f;
        decay_a_ = utli::Calc1stSmoothFilterCoeffByDecayRate((-kSilenceDb * 1000.0f) / decay_time_, update_rate_);

        auto noise_speed = param::Reverb_Speed::GetNumber(params.effects[module_idx].args);
        dp_ff_ = std::exp2(noise_speed / 12.0f);
    }
    void OnNoteOn(int note) override {}
    void OnNoteOff() override {}
private:
    std::random_device random_dev_{};
    std::uniform_real_distribution<float> urnd_;
    std::array<float, kNumPartials> latched_gains_{};
    std::array<float, kNumPartials> noise_phase_{};
    std::array<float, kNumPartials> latched_noise_{};
    std::array<float, kNumPartials> latched_noise2_{};
    float amount_{};
    float decay_time_{};
    float attack_{};
    float damp_{};
    float sample_rate_{};
    float attack_a_{};
    float decay_a_{};
    float update_rate_{};
    float inv_system_rate_{};
    float last_noise_out_{};
    float dp_ff_{};
};
}