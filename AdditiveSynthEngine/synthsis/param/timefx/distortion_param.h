#pragma once

#include "param/param.h"

namespace mana::param {
struct DistortionType : IntChoiceParam<DistortionType> {
    static constexpr auto kId = "timefx.distortion.type"sv;
    static constexpr auto kName = "type"sv;
    static constexpr auto kDefault = 0;
    enum class ParamEnum {
        kSoftClip,
        kHardClip,
        kNumEnums
    };
    static constexpr std::array kNames{
        "soft_clip"sv,
        "hard_clip"sv
    };
};

struct DistortionAmount : FloatParam<DistortionAmount> {
    static constexpr auto kId = "timefx.distortion.amount"sv;
    static constexpr auto kName = "amount"sv;
    static constexpr auto kDefault = 0.0f;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 30.0f;
    static constexpr auto kStuff = "dB"sv;
};

struct DistortionFilterType : IntChoiceParam<DistortionFilterType> {
    static constexpr auto kId = "timefx.distortion.filter_type"sv;
    static constexpr auto kName = "filter_type"sv;
    static constexpr auto kDefault = 0;
    static constexpr std::array kNames{
        "pre"sv,
        "post"sv,
        "none"sv
    };
    enum class ParamEnum {
        kPre,
        kPost,
        kNone,
        kNumEnums
    };
};

struct DistortionFilterCutoff : FloatParam<DistortionFilterCutoff> {
    static constexpr auto kId = "timefx.distortion.filter_cutoff"sv;
    static constexpr auto kName = "filter_cutoff"sv;
    static constexpr auto kDefault = 64.0f;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 150.0f;
    static constexpr auto kStuff = "st"sv;
};

struct DistortionFilterMode : FloatChoiceParam<DistortionFilterMode> {
    static constexpr auto kId = "timefx.distortion.filter_mode"sv;
    static constexpr auto kName = "filter_mode"sv;
    static constexpr auto kDefault = 0;
    enum class ParamEnum {
        kLowpass,
        kBandpass,
        kHighpass,
        kNumEnums
    };
    static constexpr std::array kNames{
        "lowpass"sv,
        "bandpass"sv,
        "highpass"sv
    };
};
}