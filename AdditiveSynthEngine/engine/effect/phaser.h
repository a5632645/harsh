#pragma once

#include <numbers>
#include <algorithm>
#include <gcem.hpp>
#include "param/param.h"
#include "param/effect_param.h"
#include "engine/IProcessor.h"
#include "engine/VolumeTable.hpp"
#include "engine/EngineConfig.h"
#include "utli/warp.h"
#include "utli/convert.h"

namespace mana {
inline static constexpr float MySimpleAbs(float x) {
    return x >= 0.0f ? x : -x;
}

static constexpr auto kSineTable = NormalizeTableByPower(MakeNormalizeTable<kNumPartials>(
    [](float v) {
    return gcem::sqrt(0.5f * gcem::cos(v * std::numbers::pi_v<float> *2.0f) + 0.5f);
}));

inline static constexpr float SingeCycleTriangle(float v) {
    return MySimpleAbs(2.0f * v);
}

inline constexpr float SingeCycleBox(float x) {
    return SingeCycleTriangle(x) > 0.5f ? 1.0f : 0.0f;
}

static constexpr auto kLogSineTable = MakeNormalizeTable<kNumPartials>(
    [](float v) {
    auto cos_val = gcem::cos(std::numbers::pi_v<float> *2.0f * v);
    auto vv = cos_val * 0.5f + 0.5f;
    auto up_db = 0.0f;
    auto down_db = -60.0f;
    auto map_db = std::lerp(down_db, up_db, vv);
    return utli::cp::DbToGain(map_db) * 1.2f;
});

static constexpr auto kLogTriTable = MakeNormalizeTable<kNumPartials>(
    [](float v) {
    if (v > 0.5f) {
        v = 1.0f - v;
    }
    v *= 2.0f; // map to 0..1
    auto up_db = 0.0f;
    auto down_db = -60.0f;
    auto map_db = std::lerp(up_db, down_db, v);
    return utli::cp::DbToGain(map_db) * 1.5f;
});

static constexpr auto kLogNarrowTable = MakeNormalizeTable<kNumPartials>(
    [](float v) {
    auto cos_val = gcem::cos(std::numbers::pi_v<float> *2.0f * v);
    if (cos_val < 0.0f) {
        return 0.0f;
    }
    auto up_db = 0.0f;
    auto down_db = -60.0f;
    auto map_db = std::lerp(down_db, up_db, cos_val);
    return utli::cp::DbToGain(map_db) * 1.3f;
});

static float PhaserShapeVal(param::Phaser_Shape::ParamEnum s, float nor_x) {
    auto nor_sym_x = nor_x - 0.5f;
    using enum param::Phaser_Shape::ParamEnum;
    switch (s) {
    case kTri:
        return SingeCycleTriangle(nor_sym_x);
    case kSine:
        return utli::warp::GetRangeByNorIdx(kSineTable, nor_x);
    case kLogSine:
        return utli::warp::GetRangeByNorIdx(kLogSineTable, nor_x);
    case kLogTri:
        return utli::warp::GetRangeByNorIdx(kLogTriTable, nor_x);
    case kLogNarrow:
        return utli::warp::GetRangeByNorIdx(kLogNarrowTable, nor_x);
    case kBox:
        return SingeCycleBox(nor_sym_x);
    default:
        return {};
    }
}
}

namespace mana {
class Phaser : public IProcessor {
public:
    void Init(float sample_rate, float update_rate) override {
        sample_rate_ = sample_rate;
    }

    void Process(Partials& partials) override {
        for (int i = 0; i < kNumPartials; ++i) {
            auto freq = partials.freqs[i] - partials.base_frequency;
            auto pitch = partials.pitches[i] - partials.base_pitch;

            auto mode1_ph = GetPhase(first_mode_, i, freq, pitch);
            auto mode2_ph = GetPhase(second_mode_, i, freq, pitch);
            auto mode1_gain = std::lerp(PhaserShapeVal(first_shape_, mode1_ph),
                                        PhaserShapeVal(second_shape_, mode1_ph),
                                        shape_fraction_);
            auto mode2_gain = std::lerp(PhaserShapeVal(first_shape_, mode2_ph),
                                        PhaserShapeVal(second_shape_, mode2_ph),
                                        shape_fraction_);
            auto org_gain = std::lerp(mode1_gain, mode2_gain, mode_fraction_);
            auto gain = (1.0f - mix_) + mix_ * org_gain;
            partials.gains[i] *= gain;
        }
    }

    void OnUpdateTick(const SynthParam& params, int skip, int module_idx) override {
        auto cycle01 = param::FloatParam<param::Phaser_Cycles>::GetNumber(params.effects[module_idx].args);
        cycles_ = cycle01 * kNumPartials / 2;
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