#pragma once

#include "param/param.h"

namespace mana::param {
struct LFO_Rate : FloatParam < LFO_Rate, [](float v) {return v * v * v; } > {
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 50.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr auto kName = "rate"sv;
    static constexpr auto kStuff = "hz"sv;
    static constexpr int kTextPrecision = 2;
};

struct LFO_Phase : FloatParam<LFO_Phase> {
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr auto kName = "phase"sv;
    static constexpr int kTextPrecision = 2;
};

struct LFO_Level : FloatParam<LFO_Level> {
    static constexpr float kMin = -1.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 1.0f;
    static constexpr auto kName = "level"sv;
    static constexpr int kTextPrecision = 2;
};

struct LFO_WaveType : IntChoiceParam<LFO_WaveType> {
    enum class ParamEnum {
        kSine = 0,
        kTri,
        kSawUp,
        kSawDown,
        kSquare,
        kRandom,
        kSampleAndHold,
        kCustom,
        kNumEnums
    };

    static constexpr std::array kNames{
        "sine"sv,
        "tri"sv,
        "saw up"sv,
        "saw down"sv,
        "square"sv,
        "random"sv,
        "S&H"sv,
        "custom"sv
    };
};
}