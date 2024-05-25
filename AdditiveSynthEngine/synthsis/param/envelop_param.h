#pragma once

#include "param/param.h"

namespace mana::param {
struct Env_Attack : FloatParam <Env_Attack> {
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 5000.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr auto kName = "attack"sv;
    static constexpr auto kStuff = "ms"sv;
    static constexpr int kTextPrecision = 0;
};

struct Env_Decay : FloatParam<Env_Decay> {
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 5000.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr auto kStuff = "ms"sv;
    static constexpr auto kName = "decay"sv;
    static constexpr int kTextPrecision = 0;
};

struct Env_Sustain : FloatParam<Env_Sustain> {
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 1.0f;
    static constexpr auto kName = "sustain"sv;
    static constexpr int kTextPrecision = 2;
};

struct Env_Release : FloatParam<Env_Release> {
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 5000.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr auto kStuff = "ms"sv;
    static constexpr auto kName = "release"sv;
    static constexpr int kTextPrecision = 0;
};
}