#pragma once

#include "param/param.h"

namespace mana::param {
struct LFO_Mode : IntChoiceParam<LFO_Mode> {
    static constexpr auto kName = "mode"sv;
    static constexpr auto kIdFormater = "lfo{}.mode"sv;
    static constexpr std::array kNames{
        "reset"sv,
        "free"sv,
        "once"sv
    };
    enum class ParamEnum {
        kReset = 0,
        kFree,
        kOnce,
        kNumEnums
    };
};

struct LFO_TimeType : IntChoiceParam<LFO_TimeType> {
    static constexpr auto kName = "time_type"sv;
    static constexpr auto kIdFormater = "lfo{}.time_type"sv;
    static constexpr std::array kNames{
        "hz"sv,
        "seconds"sv,
        "beats"sv
    };
    enum class ParamEnum {
        kHz = 0,
        kSeconds,
        kBeats,
        kNumEnums
    };
};

struct LFO_SecondRate : FloatParam<LFO_SecondRate> {
    static constexpr auto kName = "rate"sv;
    static constexpr float kMin = 0.001f;
    static constexpr float kMax = 100.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr auto kStuff = "s"sv;
    static constexpr int kTextPrecision = 3;
};

struct LFO_BeatRate : FloatChoiceParam<LFO_BeatRate> {
    static constexpr auto kName = "rate"sv;
    static constexpr std::array kNames{
        "32/1"sv,
        "16/1"sv,
        "8/1"sv,
        "4/1"sv,
        "2/1"sv,
        "1/1"sv,
        "1/2"sv,
        "1/4"sv,
        "1/8"sv,
        "1/16"sv,
        "1/32"sv
    };
    enum class ParamEnum {
        k32_1 = 0,
        k16_1,
        k8_1,
        k4_1,
        k2_1,
        k1_1,
        k1_2,
        k1_4,
        k1_8,
        k1_16,
        k1_32,
        kNumEnums
    };
};

struct LFO_HzRate : FloatParam <LFO_HzRate> {
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 100.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr auto kName = "rate"sv;
    static constexpr auto kStuff = "hz"sv;
    static constexpr int kTextPrecision = 2;
};

struct LFO_Phase : FloatParam<LFO_Phase> {
    static constexpr auto kName = "start_phase"sv;
    static constexpr auto kIdFormater = "lfo{}.phase"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 2;
};

struct LFO_Level : FloatParam<LFO_Level> {
    static constexpr auto kIdFormater = "lfo{}.level"sv;
    static constexpr float kMin = -1.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 1.0f;
    static constexpr auto kName = "level"sv;
    static constexpr int kTextPrecision = 2;
};

struct LFO_WaveType : IntChoiceParam<LFO_WaveType> {
    static constexpr auto kName = "wavetype"sv;
    static constexpr auto kIdFormater = "lfo{}.wavetype"sv;

    enum class ParamEnum {
        kSine = 0,
        kTri,
        kSawUp,
        kSawDown,
        kSquare,
        kCustom,
        kNumEnums
    };

    static constexpr std::array kNames{
        "sine"sv,
        "tri"sv,
        "saw up"sv,
        "saw down"sv,
        "square"sv,
        "custom"sv
    };
};
}