#pragma once

#include <numbers>
#include <algorithm>
#include <gcem.hpp>
#include "param/param.h"
#include "param/effect_param.h"
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
        return utli::warp::AtNormalizeIndex(kSineTable, nor_x);
    case kLogSine:
        return utli::warp::AtNormalizeIndex(kLogSineTable, nor_x);
    case kLogTri:
        return utli::warp::AtNormalizeIndex(kLogTriTable, nor_x);
    case kLogNarrow:
        return utli::warp::AtNormalizeIndex(kLogNarrowTable, nor_x);
    case kBox:
        return SingeCycleBox(nor_sym_x);
    default:
        return {};
    }
}
}

namespace mana {
class Phaser : public EffectBase {
public:
    void Init(float sample_rate, float update_rate) override {
        sample_rate_ = sample_rate;
        inv_sample_rate_ = 1.0f / sample_rate;
        inv_update_rate_ = 1.0f / update_rate;
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

    void OnUpdateTick(EffectParams& args) override {
        auto cycle01 = helper::GetAlterParamValue(args.args, param::Phaser_Cycles{});
        phaser_cycles_ = cycle01 * kNumPartials / 4;
        flanger_cycles_ = cycle01 * kNumPartials / 2;
        harmonic_cycles_ = cycle01 * kNumPartials;

        std::tie(first_shape_, second_shape_, shape_fraction_) = param::Phaser_Shape::GetInterpIndex(args.args[param::Phaser_Shape::kArgIdx]->Get01Value());
        std::tie(first_mode_, second_mode_, mode_fraction_) = param::Phaser_Mode::GetInterpIndex(args.args[param::Phaser_Mode::kArgIdx]->Get01Value());

        mix_ = helper::GetAlterParamValue(args.args, param::Phaser_Mix{});
        pinch_ = helper::GetAlterParamValue(args.args, param::Phaser_Pinch{});
        barber_rate_ = helper::GetAlterParamValue(args.args, param::Phaser_BarberRate{});

        float val{};
        float phase_inc = barber_rate_ * inv_update_rate_;
        lfo_phase_ += phase_inc;
        lfo_phase_ = std::modf(lfo_phase_, &val);
    }
    void OnNoteOn(int note) override {}
    void OnNoteOff() override {}
private:
    float GetPhase(param::Phaser_Mode::ParamEnum mode, int harmonic_no, float freq, float pitch) {
        float temp{};
        float nor_phase = {};

        switch (mode) {
        case mana::param::Phaser_Mode::ParamEnum::kSemitone:
            nor_phase = std::modf(std::abs(pitch / 140.0f) * phaser_cycles_, &temp);
            break;
        case mana::param::Phaser_Mode::ParamEnum::kHz:
            nor_phase = std::modf(std::abs(freq * inv_sample_rate_) * flanger_cycles_, &temp);
            break;
        case mana::param::Phaser_Mode::ParamEnum::kHarmonic:
            nor_phase = std::modf(harmonic_no * harmonic_cycles_ / static_cast<float>(kNumPartials), &temp);
            break;
        default:
            break;
        }

        // warping
        auto warp_phase = ParabolaWarp(nor_phase, pinch_);

        // warp 01
        return std::modf(warp_phase + lfo_phase_ + 1.0f, &temp);
    }

    static float ParabolaWarp(float x, float w) {
        return (w + 1.0f - w * std::abs(x)) * x;
    }

    float phaser_cycles_;
    float flanger_cycles_;
    float harmonic_cycles_;
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
    float inv_sample_rate_;
    float inv_update_rate_;
    float lfo_phase_;
};
}