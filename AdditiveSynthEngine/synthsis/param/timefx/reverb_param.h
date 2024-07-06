#pragma once

#include "param/param.h"
#include <gcem.hpp>

namespace mana::param {
struct TimeReverb_ReverbType : IntChoiceParam<TimeReverb_ReverbType> {
    static constexpr auto kId = "timefx.reverb.reverbtype";
    static constexpr auto kName = "reverbtype";
    enum class ParamEnum {
        kFreeverb,
        kFdnReverb,
        kNumEnums
    };
    static constexpr std::array kNames{
        "freeverb"sv,
        "fdnreverb"sv
    };
};
struct TimeReverb_Damp : FloatParam<TimeReverb_Damp> {
    static constexpr auto kId = "timefx.reverb.damp";
    static constexpr auto kName = "damp";
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 0.0f;
};
struct TimeReverb_Dry : FloatParam<TimeReverb_Dry> {
    static constexpr auto kId = "timefx.reverb.dry";
    static constexpr auto kName = "dry";
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 1.0f;
};
struct TimeReverb_RoomSize : FloatParam<TimeReverb_RoomSize> {
    static constexpr auto kId = "timefx.reverb.roomsize";
    static constexpr auto kName = "roomsize";
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 0.0f;
};
struct TimeReverb_Wet : FloatParam<TimeReverb_Wet> {
    static constexpr auto kId = "timefx.reverb.wet";
    static constexpr auto kName = "wet";
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 0.0f;
};
struct TimeReverb_Width : FloatParam<TimeReverb_Width> {
    static constexpr auto kId = "timefx.reverb.width";
    static constexpr auto kName = "width";
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 0.0f;
};

// fdn reverb
struct TimeReverb_Distribution : FloatParam<TimeReverb_Distribution> {
    static constexpr auto kId = "timefx.reverb.distribution";
    static constexpr auto kName = "distribution";
    static constexpr auto kMin = 0.03f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 0.03f;
};
struct TimeReverb_Separate : FloatParam<TimeReverb_Separate> {
    static constexpr auto kId = "timefx.reverb.separate";
    static constexpr auto kName = "separate";
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 0.0f;
};
struct TimeReverb_Feedback2 : FloatParam<TimeReverb_Feedback2> {
    static constexpr auto kId = "timefx.reverb.feedback2";
    static constexpr auto kName = "feedback2";
    static constexpr auto kMin = 0.01f;
    static constexpr auto kMax = 0.99f;
    static constexpr auto kDefault = 0.01f;
};
struct TimeReverb_Feedback3 : FloatParam<TimeReverb_Feedback3> {
    static constexpr auto kId = "timefx.reverb.feedback3";
    static constexpr auto kName = "feedback3";
    static constexpr auto kMin = 0.01f;
    static constexpr auto kMax = 1.0f / 12.0f;
    static constexpr auto kDefault = 0.01f;
};
struct TimeReverb_FreqShift : FloatParam<TimeReverb_FreqShift> {
    static constexpr auto kId = "timefx.reverb.freq_shift";
    static constexpr auto kName = "freq_shift";
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 0.0f;
};
struct TimeReverb_FdnMatrix : IntChoiceParam<TimeReverb_FdnMatrix> {
    static constexpr auto kId = "timefx.reverb.fdn_matrix";
    static constexpr auto kName = "fdn_matrix";
    enum class ParamEnum {
        kHadamard,
        kRandom,
        kHouseholder,
        kNumEnums
    };
    static constexpr std::array kNames{
        "hadamard"sv,
        "random"sv,
        "householder"sv
    };
};
}