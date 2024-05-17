#pragma once

#include "param/param.h"

namespace mana::param {
using namespace std::string_view_literals;

struct Filter_Type : IntChoiceParam<Filter_Type> {
    enum class ParamEnum {
        kLowpass = 0,
        kHighpass,
        kBandpass,
        kBandstop,
        kCombFilter,
        kPhaser,
        kNumEnums
    };

    static constexpr std::array kNames{
        "lowpass"sv,
        "highpass"sv,
        "bandpass"sv,
        "bandstop"sv,
        "comb"sv,
        "phaser"sv
    };
};

// ============================================================
// low pass
// ============================================================
struct Filter_Cutoff : FloatParam<Filter_Cutoff> {
    static constexpr int kArgIdx = 0;
    static constexpr float kMin = 0.1f;
    static constexpr float kMax = 150.0f;
    static constexpr float kDefault = 65.0f;
    static constexpr int kTextPrecision = 1;
    static constexpr auto kName = "cutoff"sv;
    static constexpr auto kStuff = "st"sv;
};

struct Filter_Resonance : FloatParam<Filter_Resonance> {
    static constexpr int kArgIdx = 1;
    static constexpr auto kName = "resonance"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 20.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 1;
    static constexpr auto kStuff = "dB"sv;
};

struct Filter_Slope : FloatParam<Filter_Slope> {
    static constexpr int kArgIdx = 2;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 48.0f;
    static constexpr float kDefault = 6.0f;
    static constexpr int kTextPrecision = 0;
    static constexpr auto kName = "slope"sv;
    static constexpr auto kStuff = "dB"sv;
};

struct Filter_BandWidth : FloatParam<Filter_BandWidth> {
    static constexpr int kArgIdx = 4;
    static constexpr float kMin = 0.1f;
    static constexpr float kMax = 24.0f;
    static constexpr float kDefault = 12.0f;
    static constexpr int kTextPrecision = 1;
    static constexpr auto kName = "bw"sv;
    static constexpr auto kStuff = "st"sv;
};

struct Filter_Knee : FloatParam<Filter_Knee> {
    static constexpr auto kName = "knee"sv;
    static constexpr int kArgIdx = 5;
    static constexpr float kMin = 0.1f;
    static constexpr float kMax = 12.0f;
    static constexpr float kDefault = 0.1f;
    static constexpr int kTextPrecision = 1;
};

struct Filter_KeyTracking : FloatParam<Filter_KeyTracking> {
    static constexpr int kArgIdx = 3;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 1;
    static constexpr auto kName = "key_track"sv;
};

// ============================================================
// comb filter
// ============================================================
struct Filter_CombShape : FloatParam<Filter_CombShape> {
    static constexpr int kArgIdx = 1;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 2;
    static constexpr auto kName = "shape"sv;
};

struct Filter_CombPhase : FloatParam<Filter_CombPhase> {
    static constexpr int kArgIdx = 2;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 3;
    static constexpr auto kName = "phase"sv;
};

struct Filter_CombDepth : FloatParam<Filter_CombDepth> {
    static constexpr int kArgIdx = 4;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 1.0f;
    static constexpr int kTextPrecision = 2;
    static constexpr auto kName = "depth"sv;
};

struct Filter_CombPhaser : FloatParam<Filter_CombPhaser> {
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
struct Filter_PhaserNotches : FloatParam<Filter_PhaserNotches> {
    static constexpr int kArgIdx = 3;
    static constexpr float kMin = 1.0f;
    static constexpr float kMax = 8.0f;
    static constexpr float kDefault = 1.0f;
    static constexpr int kTextPrecision = 0;
    static constexpr auto kName = "notch"sv;
};

struct Filter_PhaserWidth : FloatParam<Filter_PhaserWidth> {
    static constexpr int kArgIdx = 5;
    static constexpr float kMin = 0.1f;
    static constexpr float kMax = 120.0f;
    static constexpr float kDefault = 1.0f;
    static constexpr int kTextPrecision = 1;
    static constexpr auto kName = "width"sv;
    static constexpr auto kStuff = "st"sv;
};

using Filter_PhaserDepth = Filter_CombDepth;
using Filter_PhaserShape = Filter_CombShape;
using Filter_PhaserPhase = Filter_CombPhase;
}