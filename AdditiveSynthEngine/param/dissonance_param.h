#pragma once

#include <array>
#include "param/param.h"

namespace mana::param {
struct DissonanceType : IntChoiceParam<DissonanceType> {
    enum class ParamEnum {
        kString = 0,
        kHarmonicStretch,
        kSemitoneSpace,
        kNumEnums
    };

    static constexpr auto kName = "type"sv;
    static constexpr std::array kNames = {
        "string"sv,"harmonic"sv,"st space"sv
    };
};

// harmonic stretch
struct HarmonicStrech : FloatParam<HarmonicStrech> {
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "stretch"sv;
    static constexpr auto kMin = 1.0f;
    static constexpr auto kMax = 4.0f;
    static constexpr auto kDefault = 1.0f;
    static constexpr auto kTextPrecision = 3;
};

// =========================================================
// semitone space
// =========================================================
struct SemitoneSpace : FloatParam<SemitoneSpace> {
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "pitch"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 24.0f;
    static constexpr auto kDefault = 1.0f;
    static constexpr auto kTextPrecision = 1;
};

// =========================================================
// string
// =========================================================
struct StringDissStretch : FloatParam<StringDissStretch> {
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "string"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 0.1f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 5;
};

struct StringMultiRatio : FloatChoiceParam<StringMultiRatio> {
    enum class ParamEnum {
        k1 = 0,
        k10,
        k100,
        k1000,
        k10000,
        kNumEnums
    };

    static constexpr std::array kNames = {
        "1.0"sv, "0.1"sv, "0.01"sv, "0.001"sv, "0.0001"sv
    };
    static constexpr auto kName = "scale"sv;
    static constexpr int kArgIdx = 1;
};
}