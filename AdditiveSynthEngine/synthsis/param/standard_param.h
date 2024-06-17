#pragma once

#include "param/param.h"

namespace mana::param {
struct PitchBend : FloatParam<PitchBend> {
    static constexpr auto kName = "pitch_bend"sv;
    static constexpr float kMin = -60.0f;
    static constexpr float kMax = 60.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr auto kStuff = "st"sv;
    static constexpr int kTextPrecision = 1;
};

struct OutputGain : FloatParam<OutputGain> {
    static constexpr auto kName = "out.gain"sv;
    static constexpr float kMin = -60.0f;
    static constexpr float kMax = 40.0f;
    static constexpr float kDefault = -6.0f;
    static constexpr auto kStuff = "dB"sv;
    static constexpr int kTextPrecision = 1;
};

struct PhaseType : IntChoiceParam<PhaseType> {
    static constexpr auto kName = "out.gain"sv;

    enum class ParamEnum {
        kResetOnce = 0,
        kContinue,
        kDispersion,
        kSpectralRandom,
        kPowDistribute,
        kNumEnums
    };

    static constexpr std::array kNames{
        "reset once"sv,
        "continue"sv,
        "dispersion"sv,
        "sp.random"sv,
        "pow dist"sv,
    };
};

struct PhaseDispersion_Amount : FloatParam<PhaseDispersion_Amount> {
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "amount"sv;
    static constexpr float kMin = -1.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 2;
};

struct PhaseDispersion_Warp : FloatParam<PhaseDispersion_Warp> {
    static constexpr int kArgIdx = 1;
    static constexpr auto kName = "warp"sv;
    static constexpr float kMin = -1.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 2;
};

struct PhaseSpRandom_Amount : FloatParam<PhaseSpRandom_Amount> {
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "amount"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 2;
};

struct PhaseSpRandom_Smooth : FloatParam<PhaseSpRandom_Smooth> {
    static constexpr int kArgIdx = 1;
    static constexpr auto kName = "smooth"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 2;
};

struct PhasePowDist_Pow : FloatParam<PhasePowDist_Pow> {
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "pow"sv;
    static constexpr float kMin = 1.01f;
    static constexpr float kMax = 1.99f;
    static constexpr float kDefault = 1.3f;
    static constexpr int kTextPrecision = 2;
};
}