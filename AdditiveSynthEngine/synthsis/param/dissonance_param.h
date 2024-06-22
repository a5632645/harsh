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
        kDispersion,
        kPitchQuantize,
        kPrism,
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
        "dispersion"sv,
        "p.quantize"sv,
        "prism"sv
    };
};

// =========================================================
// harmonic strech
// =========================================================
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
    static constexpr int kTextPrecision = 3;
};

struct String_Quantize : FloatParam<String_Quantize> {
    static constexpr int kArgIdx = 1;
    static constexpr auto kName = "quantize"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 2;
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
// fake unison
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

// =========================================================
// dispersion
// =========================================================
struct Dispersion_Amount : FloatParam<Dispersion_Amount> {
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "amount"sv;
    static constexpr auto kMin = -4.0f;
    static constexpr auto kMax = 4.0f;
    static constexpr auto kDefault = 0.0f;
    static constexpr auto kTextPrecision = 1;
};

struct Dispersion_Warp : FloatParam<Dispersion_Warp> {
    static constexpr int kArgIdx = 1;
    static constexpr auto kName = "warp"sv;
    static constexpr auto kMin = -1.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 0.0f;
    static constexpr auto kTextPrecision = 2;
};

// =========================================================
// pitch quantize
// =========================================================
struct PitchQuantize_Amount : FloatParam<PitchQuantize_Amount> {
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "amount"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 0.0f;
    static constexpr auto kTextPrecision = 2;
};

// =========================================================
// prism
// =========================================================
struct Prism_Amount : FloatParam<Prism_Amount> {
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "amount"sv;
    static constexpr auto kMin = -1.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 0.0f;
    static constexpr auto kTextPrecision = 2;
};

struct Prism_Morph : FloatParam<Prism_Morph> {
    static constexpr int kArgIdx = 1;
    static constexpr auto kName = "morph"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 0.0f;
    static constexpr auto kTextPrecision = 2;
};
}