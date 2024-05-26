#pragma once

#include "param/param.h"

// =========================================================
// unison
// =========================================================
namespace mana::param {
struct Unison_Type : IntChoiceParam<Unison_Type> {
    static constexpr auto kName = "num_voice"sv;

    enum class ParamEnum {
        kPUniform = 0,
        kHzUniform,
        kRandom,
        kNumEnums
    };

    static constexpr std::array kNames{
        "p.uniform"sv,
        "hz.uniform"sv,
        "random"sv
    };
};

struct Unison_NumVoice : IntParam<Unison_NumVoice> {
    static constexpr auto kName = "num_voice"sv;
    static constexpr int kMin = 1;
    static constexpr int kMax = 9;
    static constexpr int kDefault = 1;
    static constexpr int kTextPrecision = 0;
};

struct Unison_Pitch : FloatParam<Unison_Pitch> {
    static constexpr auto kName = "pitch"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 0.5f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 3;
    static constexpr auto kStuff = "st";
};

struct Unison_Phase : FloatParam<Unison_Phase> {
    static constexpr auto kName = "phase"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 1;
};

struct Unison_Pan : FloatParam<Unison_Pan> {
    static constexpr auto kName = "pan"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 1;
};
}