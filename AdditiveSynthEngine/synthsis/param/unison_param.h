#pragma once

#include "param/param.h"

// =========================================================
// unison
// =========================================================
namespace mana::param {
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

struct Unison_Morph : FloatParam<Unison_Morph> {
    static constexpr auto kName = "morph"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 1;
};

struct Unison_Wave : FloatParam<Unison_Wave> {
    static constexpr auto kName = "wave"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 1;
};

struct Unison_Randomness : FloatParam<Unison_Randomness> {
    static constexpr auto kName = "random"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 1;
};
}