#pragma once

#include "param/param.h"

namespace mana::param {
struct Chrous_RateMode : IntChoiceParam<Chrous_RateMode> {
    static constexpr auto kId = "timefx.chrous.rateode"sv;
    static constexpr auto kName = "ratemode"sv;
    static constexpr auto kDefault = 0;
    enum class ParamEnum {
        kHz,
        kBeats,
        kNumEnums
    };
    static constexpr std::array kNames{
        "hz"sv,
        "beats"sv
    };
};

struct Chrous_RateHz : FloatParam<Chrous_RateHz> {
    static constexpr auto kName = "rate"sv;
    static constexpr auto kDefault = 0.5f;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 4.0f;
    static constexpr auto kStuff = "hz"sv;
    static constexpr auto kTextPrecision = 1;
};

struct Chrous_RateBeat : FloatChoiceParam<Chrous_RateBeat> {
    static constexpr auto kName = "rate"sv;
    static constexpr std::array kNames{
        "16/1"sv,
        "8/1"sv,
        "4/1"sv,
        "2/1"sv,
        "1/1"sv,
        "1/2"sv,
        "1/4"sv,
        "1/8"sv,
        "1/16"sv,
    };
    static constexpr std::array kNumbers{
        16.0f,
        8.0f,
        4.0f,
        2.0f,
        1.0f,
        1.0f / 2.0f,
        1.0f / 4.0f,
        1.0f / 8.0f,
        1.0f / 16.0f,
    };
    enum class ParamEnum {
        k16_1,
        k8_1,
        k4_1,
        k2_1,
        k1_1,
        k1_2,
        k1_4,
        k1_8,
        k1_16,
        kNumEnums
    };
};

struct Chrous_Feedback : FloatParam<Chrous_Feedback> {
    static constexpr auto kId = "timefx.chrous.feedback"sv;
    static constexpr auto kName = "Feedback"sv;
    static constexpr auto kDefault = 0.0f;
    static constexpr auto kMin = -0.99f;
    static constexpr auto kMax = 0.99f;
};

struct Chrous_Delay1 : FloatParam<Chrous_Delay1> {
    static constexpr auto kId = "timefx.chrous.delay1"sv;
    static constexpr auto kName = "delay1"sv;
    static constexpr auto kDefault = 10.0f;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 30.0f;
    static constexpr auto kStuff = "ms"sv;
};

struct Chrous_Delay2 : FloatParam<Chrous_Delay2> {
    static constexpr auto kId = "timefx.chrous.delay2"sv;
    static constexpr auto kName = "delay2"sv;
    static constexpr auto kDefault = 10.0f;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 30.0f;
    static constexpr auto kStuff = "ms"sv;
};

struct Chrous_Depth : FloatParam<Chrous_Depth> {
    static constexpr auto kId = "timefx.chrous.depth"sv;
    static constexpr auto kName = "depth"sv;
    static constexpr auto kDefault = 20.0f;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 30.0f;
    static constexpr auto kStuff = "ms"sv;
};
}