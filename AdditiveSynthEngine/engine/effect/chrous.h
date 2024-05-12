#pragma once

#include <numbers>
#include <complex>
#include "effect_base.h"
#include "param/effect_param.h"

namespace mana {
class Chorus : public EffectBase {
public:
    void Init(float sample_rate, float update_rate) override {
        sample_rate_ = sample_rate;
        inv_update_rate_ = 1.0f / update_rate;
    }

    void Process(Partials& partials) override {
        constexpr auto two_pi = std::numbers::pi_v<float> *2.0f;
        auto voice1_delay_samples = std::lerp(offset_num_samples_, offset_num_samples_ + depth_num_samples_, voice_phase_) * two_pi;
        auto voice2_delay_samples = std::lerp(offset_num_samples_, offset_num_samples_ + depth_num_samples_, voice2_phase_) * two_pi;
        for (int i = 0; i < kNumPartials; ++i) {
            auto gain_cpx = std::complex(1.0f, 0.0f)
                + std::polar(1.0f, voice1_delay_samples * partials.freqs[i])
                + std::polar(1.0f, voice2_delay_samples * partials.freqs[i]);
            auto wet_gain = std::abs(gain_cpx) / 2.0f;
            partials.gains[i] = std::lerp(partials.gains[i], partials.gains[i] * wet_gain, amount_);
        }
    }

    void OnUpdateTick(EffectParams& args, CurveManager& curves) override {
        amount_ = param::Chorus_Amount::GetNumber(args.args);
        depth_num_samples_ = param::Chorus_Depth::GetNumber(args.args) * sample_rate_ / 1000.0f;
        offset_num_samples_ = param::Chorus_Offset::GetNumber(args.args) * sample_rate_ / 1000.0f;
        lfo_rate_ = param::Chorus_Speed::GetNumber(args.args);

        // update lfo
        float val{};
        float phase_inc = lfo_rate_ * inv_update_rate_;
        lfo_phase_ += phase_inc;
        lfo_phase_ = std::modf(lfo_phase_, &val);
        voice_phase_ = std::sin(std::numbers::pi_v<float> *lfo_phase_);
        voice2_phase_ = std::sin(std::numbers::pi_v<float> *(-lfo_phase_ + 0.5f));
    }
    void OnNoteOn(int note) override {}
    void OnNoteOff() override {}
private:
    float amount_;
    float depth_num_samples_;
    float offset_num_samples_;
    float lfo_rate_;
    float sample_rate_;
    float inv_update_rate_;
    float lfo_phase_;
    float voice_phase_;
    float voice2_phase_;
};
}