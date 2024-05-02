#pragma once

#include <array>
#include "param/param.h"

namespace mana::param {
struct DissonanceType : IntChoiceParam<DissonanceType> {
    enum class ParamEnum {
        kString = 0,
        kHarmonicStretch,
        kSemitoneSpace,
        kStaticError,
        kFakeUnison,
        kFakeUnison2,
        kNumEnums
    };

    static constexpr auto kName = "type"sv;
    static constexpr std::array kNames = {
        "string"sv,
        "harmonic"sv,
        "st space"sv,
        "static error"sv,
        "fake unison"sv,
        "fake unison2"sv,
    };
};

// harmonic stretch
struct HarmonicStrech : FloatParam<HarmonicStrech> {
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "stretch"sv;
    static constexpr auto kMin = 0.25f;
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
    static constexpr float kMax = 1.0f;
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

// =========================================================
// error
// =========================================================
struct ErrorRamp : FloatParam<ErrorRamp> {
    static constexpr int kArgIdx = 1;
    static constexpr auto kName = "ramp"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 0.0f;
    static constexpr auto kTextPrecision = 2;
};

struct ErrorRange : FloatParam<ErrorRange> {
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "range"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 4.0f;
    static constexpr auto kDefault = 0.0f;
    static constexpr auto kTextPrecision = 2;
};

// =========================================================
// error
// =========================================================
struct FakeUnisonRatio0 : FloatParam<FakeUnisonRatio0> {
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "ratio0"sv;
    static constexpr auto kMin = -24.0f;
    static constexpr auto kMax = 24.0f;
    static constexpr auto kDefault = 0.0f;
    static constexpr auto kTextPrecision = 1;
    static constexpr auto kStuff = "st";
};

struct FakeUnisonRatio1 : FloatParam<FakeUnisonRatio1> {
    static constexpr int kArgIdx = 1;
    static constexpr auto kName = "ratio1"sv;
    static constexpr auto kMin = -24.0f;
    static constexpr auto kMax = 24.0f;
    static constexpr auto kDefault = 0.0f;
    static constexpr auto kTextPrecision = 1;
    static constexpr auto kStuff = "st";
};
}