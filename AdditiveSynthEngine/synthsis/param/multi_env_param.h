#pragma once

#include "param.h"

namespace mana::param {
struct VolEnv_PreDelay : FloatParam<VolEnv_PreDelay> {
    static constexpr auto kId = "vol_env.predelay"sv;
    static constexpr auto kName = "predelay"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 5.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr auto kStuff = "s"sv;
    static constexpr int kTextPrecision = 3;
};
struct VolEnv_Attack : FloatParam<VolEnv_Attack> {
    static constexpr auto kId = "vol_env.attack"sv;
    static constexpr auto kName = "attack"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 10.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr auto kStuff = "s"sv;
    static constexpr int kTextPrecision = 3;
};
struct VolEnv_Hold : FloatParam<VolEnv_Hold> {
    static constexpr auto kId = "vol_env.hold"sv;
    static constexpr auto kName = "hold"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 10.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr auto kStuff = "s"sv;
    static constexpr int kTextPrecision = 3;
};
struct VolEnv_Decay : FloatParam<VolEnv_Decay> {
    static constexpr auto kId = "vol_env.decay"sv;
    static constexpr auto kName = "decay"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 10.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr auto kStuff = "s"sv;
    static constexpr int kTextPrecision = 3;
};
struct VolEnv_Peak : FloatParam<VolEnv_Peak> {
    static constexpr auto kId = "vol_env.peak"sv;
    static constexpr auto kName = "peak"sv;
    static constexpr float kMin = -60.0f;
    static constexpr float kMax = 0.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr auto kStuff = "dB"sv;
    static constexpr int kTextPrecision = 1;
};
struct VolEnv_Sustain : FloatParam<VolEnv_Sustain> {
    static constexpr auto kId = "vol_env.sustain"sv;
    static constexpr auto kName = "sustain"sv;
    static constexpr float kMin = -60.0f;
    static constexpr float kMax = 0.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr auto kStuff = "dB"sv;
    static constexpr int kTextPrecision = 1;
};
struct VolEnv_Release : FloatParam<VolEnv_Release> {
    static constexpr auto kId = "vol_env.release"sv;
    static constexpr auto kName = "release"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 10.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr auto kStuff = "s"sv;
    static constexpr int kTextPrecision = 3;
};
struct VolEnv_HighScale : FloatParam<VolEnv_HighScale> {
    static constexpr auto kId = "vol_env.high_scale"sv;
    static constexpr auto kName = "high_scale"sv;
    static constexpr float kMin = 0.1f;
    static constexpr float kMax = 10.0f;
    static constexpr float kDefault = 1.0f;
    static constexpr int kTextPrecision = 2;
};
}