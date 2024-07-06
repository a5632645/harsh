#pragma once

#include "param/param.h"

namespace mana::param {
struct DelayTimeMode : IntChoiceParam<DelayTimeMode> {
    static constexpr auto kId = "timefx.delay.timemode"sv;
    static constexpr auto kName = "Delay Time Mode"sv;
    static constexpr auto kDefault = 0;
    enum class ParamEnum {
        kSeconds,
        kBeats,
        kNumEnums
    };
    static constexpr std::array kNames{
        "seconds"sv,
        "beats"sv
    };
};

struct DelayTimeSeconds : FloatParam<DelayTimeSeconds> {
    static constexpr auto kId = "timefx.delay.time"sv;
    static constexpr auto kName = "Delay Time"sv;
    static constexpr auto kDefault = 0.5f;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 4.0f;
    static constexpr auto kStuff = "s"sv;
    static constexpr auto kTextPrecision = 3;
};

struct DelayTimeBeat : FloatChoiceParam<DelayTimeBeat> {
    static constexpr auto kName = "rate"sv;
    static constexpr std::array kNames{
        "4/1"sv,
        "2/1"sv,
        "1/1"sv,
        "1/2"sv,
        "1/4"sv,
        "1/8"sv,
        "1/16"sv,
        "1/32"sv,
        "1/64"sv
    };
    static constexpr std::array kNumbers{
        4.0f,
        2.0f,
        1.0f,
        1.0f / 2.0f,
        1.0f / 4.0f,
        1.0f / 8.0f,
        1.0f / 16.0f,
        1.0f / 32.0f,
        1.0f / 64.0f
    };
    enum class ParamEnum {
        k4_1,
        k2_1,
        k1_1,
        k1_2,
        k1_4,
        k1_8,
        k1_16,
        k1_32,
        k1_64,
        kNumEnums
    };
};

struct DelayFeedback : FloatParam<DelayFeedback> {
    static constexpr auto kId = "timefx.delay.feedback"sv;
    static constexpr auto kName = "Feedback"sv;
    static constexpr auto kDefault = 0.5f;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
};

struct DelayMode : IntChoiceParam<DelayMode> {
    static constexpr auto kId = "timefx.delay.mode"sv;
    static constexpr auto kName = "Delay Mode"sv;
    static constexpr auto kDefault = 0;
    enum class ParamEnum {
        kNormal,
        kPingPong,
        kNumEnums
    };
    static constexpr std::array kNames{
        "normal"sv,
        "pingpong"sv
    };
};

struct DelayFbCenter : FloatParam<DelayFbCenter> {
    static constexpr auto kId = "timefx.delay.fbcenter"sv;
    static constexpr auto kName = "Feedback Center"sv;
    static constexpr auto kDefault = 0.5f;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kStuff = "st"sv;
};

struct DelayFbBw : FloatParam<DelayFbBw> {
    static constexpr auto kId = "timefx.delay.fbbw"sv;
    static constexpr auto kName = "Feedback Bandwidth"sv;
    static constexpr auto kDefault = 0.5f;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kStuff = "st"sv;
};
}