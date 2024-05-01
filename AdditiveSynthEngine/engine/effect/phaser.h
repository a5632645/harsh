#pragma once

#include <numbers>
#include <algorithm>
#include "param/param.h"
#include "param/effect_param.h"
#include "engine/IProcessor.h"
#include "engine/VolumeTable.hpp"
#include "engine/EngineConfig.h"

namespace mana {
constexpr float MySimpleAbs(float x) {
    return x >= 0.0f ? x : -x;
}

// input are [-0.5, 0.5]
constexpr float SingeCycleBox(float x) {
    return x > -0.25 && x < 0.25 ? 1.0f : -1.0f;
}
constexpr float SingeCycleParabola(float v) {
    return -8.0f * v * v + 1.0f;
}
static const auto kSineTable = MakeNormalizeTable<kNumPartials>([](float v) {return -std::cos(v * std::numbers::pi_v<float> *2.0f); });
constexpr float SingeCycleTriangle(float v) {
    return 1.0f - MySimpleAbs(4.0f * v);
}
constexpr float SingeCycleDeep(float x) {
    x = MySimpleAbs(x);
    return -1.6f + (1.0f + 1.6f) * (1.0f / (4.5f * x + 1.0f) - 0.1f) / 0.9f;
}
static constexpr auto kDeepTable = MakeNormalizeTable<kNumPartials>([](float v) {return SingeCycleDeep(v - 0.5f); });

static float PhaserShapeVal(param::Phaser_Shape::ParamEnum s, float nor_x) {
    auto nor_sym_x = nor_x - 0.5f;
    using enum param::Phaser_Shape::ParamEnum;
    switch (s) {
    case kBox:
        return SingeCycleBox(nor_sym_x);
    case kParabola:
        return SingeCycleParabola(nor_sym_x);
    case kSine:
        return kSineTable[static_cast<size_t>(nor_x * (kSineTable.size() - 1))];
    case kTri:
        return SingeCycleTriangle(nor_sym_x);
    case kDeep:
        return SingeCycleDeep(nor_sym_x);
    default:
        return {};
    }
}
}

namespace mana {
class Phaser : public IProcessor {
public:
    void Init(float sample_rate) override {
        sample_rate_ = sample_rate;
    }

    void Process(Partials& partials) override {
        for (int i = 0; i < kNumPartials; ++i) {
            auto nor_phase = std::lerp(GetPhase(first_mode_, i, partials.freqs[i] - partials.freqs.front(), partials.pitches[i] - partials.pitches.front()),
                                       GetPhase(second_mode_, i, partials.freqs[i] - partials.freqs.front(), partials.pitches[i] - partials.pitches.front()),
                                       mode_fraction_);
            auto org_gain = std::lerp(PhaserShapeVal(first_shape_, nor_phase),
                                      PhaserShapeVal(second_shape_, nor_phase),
                                      shape_fraction_);
            auto gain = std::lerp(1.0f, 0.5f + 0.5f * org_gain, mix_);
            partials.gains[i] *= gain;
        }
    }

    void OnUpdateTick(const SynthParam& params, int skip, int module_idx) override {
        auto cycle01 = param::FloatParam<param::Phaser_Cycles>::GetNumber(params.effects[module_idx].args);
        cycles_ = cycle01 * cycle01 * kNumPartials / 2;
        {
            auto [fs, ss, f] = param::Phaser_Shape::GetInterpIndex(params.effects[module_idx].args);
            first_shape_ = fs;
            second_shape_ = ss;
            shape_fraction_ = f;
        }
        mix_ = param::FloatParam<param::Phaser_Mix>::GetNumber(params.effects[module_idx].args);
        pinch_ = param::FloatParam<param::Phaser_Pinch>::GetNumber(params.effects[module_idx].args);
        barber_rate_ = param::FloatParam<param::Phaser_BarberRate>::GetNumber(params.effects[module_idx].args);
        {
            auto [fm, sm, f] = param::Phaser_Mode::GetInterpIndex(params.effects[module_idx].args);
            first_mode_ = fm;
            second_mode_ = sm;
            mode_fraction_ = f;
        }
        UpdateLfo(skip);
    }
    void OnNoteOn(int note) override {}
    void OnNoteOff() override {}
private:
    void UpdateLfo(int skip) {
        float val{};
        float phase_inc = skip * barber_rate_ / sample_rate_;
        lfo_phase_ += phase_inc;
        lfo_phase_ = std::modf(lfo_phase_, &val);
    }

    float GetPhase(param::Phaser_Mode::ParamEnum mode, int harmonic_no, float freq, float pitch) {
        float nor_phase = {};
        switch (mode) {
        case mana::param::Phaser_Mode::ParamEnum::kSemitone:
            nor_phase = std::clamp(pitch, 0.0f, 140.0f) / 140.0f;
            break;
        case mana::param::Phaser_Mode::ParamEnum::kHz:
            nor_phase = std::clamp(freq, 0.0f, 1.0f);
            break;
        case mana::param::Phaser_Mode::ParamEnum::kHarmonic:
            nor_phase = harmonic_no / static_cast<float>(kNumPartials);
            break;
        default:
            break;
        }

        // warping
        auto warp_phase = ParabolaWarp(nor_phase, pinch_);

        // warp 01
        float temp{};
        return std::modf(warp_phase * cycles_ + lfo_phase_ + 1.0f, &temp);
    }

    static float ParabolaWarp(float x, float w) {
        return (w + 1.0f - w * std::abs(x)) * x;
    }

    float cycles_;
    param::Phaser_Shape::ParamEnum first_shape_;
    param::Phaser_Shape::ParamEnum second_shape_;
    float shape_fraction_;
    float mix_;
    float pinch_;
    float barber_rate_;
    param::Phaser_Mode::ParamEnum first_mode_;
    param::Phaser_Mode::ParamEnum second_mode_;
    float mode_fraction_;

    float sample_rate_;
    float lfo_phase_;
};
}