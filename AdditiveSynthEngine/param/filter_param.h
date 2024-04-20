#pragma once

#include <string_view>
#include <array>

namespace mana::param {
using namespace std::string_view_literals;

struct Filter_Type {
    enum class TypeEnum {
        kLowpass = 0,
        kCombFilter,
        kPhaser,
        kNumEnums
    };

    static constexpr auto kNames = std::to_array({
        "lowpass"sv, "comb"sv, "phaser"sv
                                                 });
};

// ============================================================
// low pass
// ============================================================
struct Filter_Cutoff { // arg0
    static constexpr float kMin = 0.1f;
    static constexpr float kMax = 140.0f;
    static constexpr float kDefault = 65.0f;
    static constexpr int kTextPrecision = 1;
    static constexpr auto kName = "cutoff"sv;
    static constexpr auto kStuff = "st"sv;
};

struct Filter_Resonance { // arg1
    // Todo
    static constexpr auto kName = "resonance"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 1;
    static constexpr auto kStuff = "dB"sv;
};

struct Filter_Slope { // arg2
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 80.0f;
    static constexpr float kDefault = 20.0f;
    static constexpr int kTextPrecision = 0;
    static constexpr auto kName = "slope"sv;
    static constexpr auto kStuff = "dB"sv;
};

struct Filter_KeyTracking { // arg3
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 1;
    static constexpr auto kName = "key_track"sv;
};

// ============================================================
// comb filter
// ============================================================
struct Filter_CombShape { // arg1
    static constexpr int kArgIdx = 1;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 2;
    static constexpr auto kName = "shape"sv;
};

struct Filter_CombPhase { // arg2
    static constexpr int kArgIdx = 2;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 3;
    static constexpr auto kName = "phase"sv;
};

struct Filter_CombDepth { // arg4
    static constexpr int kArgIdx = 4;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 1.0f;
    static constexpr int kTextPrecision = 2;
    static constexpr auto kName = "depth"sv;
};

struct Filter_CombPhaser { // arg5
    static constexpr int kArgIdx = 5;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 2;
    static constexpr auto kName = "phaser"sv;
};

// ============================================================
// phaser
// ============================================================
struct Filter_PhaserNotches { // arg3
    static constexpr int kArgIdx = 3;
    static constexpr float kMin = 1.0f;
    static constexpr float kMax = 16.0f;
    static constexpr float kDefault = 1.0f;
    static constexpr int kTextPrecision = 0;
    static constexpr auto kName = "notch"sv;
};

struct Filter_PhaserWidth { // arg5
    static constexpr int kArgIdx = 5;
    static constexpr float kMin = 0.1f;
    static constexpr float kMax = 24.0f;
    static constexpr float kDefault = 1.0f;
    static constexpr int kTextPrecision = 1;
    static constexpr auto kName = "width"sv;
    static constexpr auto kStuff = "st"sv;
};

using Filter_PhaserDepth = Filter_CombDepth;
using Filter_PhaserShape = Filter_CombShape;
using Filter_PhaserPhase = Filter_CombPhase;
}