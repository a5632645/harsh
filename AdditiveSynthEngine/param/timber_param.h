#pragma once

#include <string_view>
#include <array>
#include "param.h"

namespace mana::param {
using namespace std::string_view_literals;

struct TimberType : IntChoiceParam<TimberType> {
    enum class ParamEnum {
        kDualSaw = 0,
        kSync,
        kNoise,
        kPwm,
        kNumEnums
    };

    static constexpr std::array kNames{
        "dual_saw"sv,
        "sync"sv,
        "noise"sv,
        "pwm"sv,
    };
};

struct Timber_Osc2Shift : FloatParam<Timber_Osc2Shift> {
    static constexpr auto kName = "ratio"sv;
    static constexpr float kMin = 1.0f;
    static constexpr float kMax = 8.0f;
    static constexpr float kDefault = 1.0f;
    static constexpr int kTextPrecision = 0;
};

struct Timber_Osc2Beating : FloatParam<Timber_Osc2Beating> {
    static constexpr auto kName = "beating"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 1;
    static constexpr auto kStuff = "hz"sv;
};

struct Timber_OscGain : FloatParam<Timber_OscGain> {
    static constexpr auto kName = "amp"sv;
    static constexpr float kMin = -40.0f;
    static constexpr float kMax = 0.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 1;
    static constexpr auto kStuff = "dB";
};
// =========================================================
// unison
// =========================================================
struct Unison_Pitch : FloatParam<Unison_Pitch> {
    static constexpr auto kName = "pitch"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 1;
    static constexpr auto kStuff = "st";
};

struct Unison_Phase : FloatParam<Unison_Phase> {
    static constexpr auto kName = "phase"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 1;
};

struct Unison_Pan : FloatParam<Unison_Pan> {
    static constexpr auto kName = "pan"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 1;
};

// =========================================================
// dual saw
// =========================================================
struct DualSaw_Ratio : FloatParam<DualSaw_Ratio> {
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "ratio"sv;
    static constexpr float kMin = 1.0f;
    static constexpr float kMax = 8.0f;
    static constexpr float kDefault = 1.0f;
    static constexpr int kTextPrecision = 0;
};

struct DualSaw_BeatingRate : FloatParam<DualSaw_BeatingRate> {
    static constexpr int kArgIdx = 1;
    static constexpr auto kName = "beating"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 10.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 3;
    static constexpr auto kStuff = "hz"sv;
};

struct DualSaw_SawSquare : FloatParam<DualSaw_SawSquare> {
    static constexpr int kArgIdx = 2;
    static constexpr auto kName = "saw_square"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 1.0f;
    static constexpr int kTextPrecision = 2;
};

struct DualSaw_SecondAmp : FloatParam<DualSaw_SecondAmp> {
    static constexpr int kArgIdx = 3;
    static constexpr auto kName = "sec_amp"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 1.0f;
    static constexpr int kTextPrecision = 2;
};

// =========================================================
// sync
// =========================================================
struct Sync_WaveShape : FloatChoiceParam<Sync_WaveShape> {
    enum class ParamEnum {
        kSine = 0,
        kTriangle,
        kSquare,
        kSaw,
        kNumEnums
    };

    static constexpr std::array kNames{
        "sine"sv,"tri"sv,"square"sv,"saw"sv
    };
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "shape"sv;
};

struct Sync_Sync : FloatParam<Sync_Sync> {
    static constexpr int kArgIdx = 1;
    static constexpr auto kName = "sync"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 12.0f * 5.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr auto kStuff = "st"sv;
    static constexpr int kTextPrecision = 1;
};

// =========================================================
// noise
// =========================================================
struct Noise_Dynamic : FloatParam<Noise_Dynamic> {
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "dynamic"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 1.0f;
    static constexpr int kTextPrecision = 0;
};

struct Noise_Color : FloatParam<Noise_Color> {
    static constexpr int kArgIdx = 1;
    static constexpr auto kName = "color"sv;
    static constexpr float kMin = -12.0f;
    static constexpr float kMax = 0.0f;
    static constexpr float kDefault = -6.0f;
    static constexpr int kTextPrecision = 1;
};

struct Noise_Seed : FloatParam<Noise_Seed> {
    static constexpr int kArgIdx = 2;
    static constexpr auto kName = "seed"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 100.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 0;
};

// =========================================================
// pwm
// =========================================================
struct Pwm_Width : FloatParam<Pwm_Width> {
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "width"sv;
    static constexpr float kMin = -1.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 2;
};

struct Pwm_Tilt : FloatParam<Pwm_Tilt> {
    static constexpr int kArgIdx = 1;
    static constexpr auto kName = "tilt"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 2.0f;
    static constexpr float kDefault = 1.0f;
    static constexpr int kTextPrecision = 2;
};
}