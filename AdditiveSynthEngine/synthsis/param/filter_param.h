#pragma once

#include "param/param.h"

namespace mana::param {
using namespace std::string_view_literals;

static constexpr int kNumFilterArgs = 8;

struct Filter_Type : IntChoiceParam<Filter_Type> {
    static constexpr auto kName = "type"sv;

    enum class ParamEnum {
        kLowpass = 0,
        kHighpass,
        kBandpass,
        kBandstop,
        kCombFilter,
        kPhaser,
        kFormant,
        kNumEnums
    };

    static constexpr std::array kNames{
        "lowpass"sv,
        "highpass"sv,
        "bandpass"sv,
        "bandstop"sv,
        "comb"sv,
        "phaser"sv,
        "formant"sv
    };
};

// ============================================================
// low pass,high pass,band pass,band stop
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

struct Filter_Slope : FloatParam<Filter_Slope> {
    static constexpr int kArgIdx = 1;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 48.0f;
    static constexpr float kDefault = 6.0f;
    static constexpr int kTextPrecision = 0;
    static constexpr auto kName = "slope"sv;
    static constexpr auto kStuff = "dB"sv;
};

struct Filter_Knee : FloatParam<Filter_Knee> {
    static constexpr auto kName = "knee"sv;
    static constexpr int kArgIdx = 2;
    static constexpr float kMin = 0.1f;
    static constexpr float kMax = 12.0f;
    static constexpr float kDefault = 0.1f;
    static constexpr int kTextPrecision = 1;
};

struct Filter_BandWidth : FloatParam<Filter_BandWidth> {
    static constexpr int kArgIdx = 3;
    static constexpr float kMin = 0.1f;
    static constexpr float kMax = 24.0f;
    static constexpr float kDefault = 12.0f;
    static constexpr int kTextPrecision = 1;
    static constexpr auto kName = "bw"sv;
    static constexpr auto kStuff = "st"sv;
};

struct Filter_ResonanceType : FloatChoiceParam<Filter_ResonanceType> {
    static constexpr int kArgIdx = 4;
    static constexpr auto kName = "type"sv;

    enum class ParamEnum {
        kRamp = 0,
        kCos,
        kParabola,
        kPhaser,
        kNumEnums
    };

    static constexpr std::array kNames{
        "ramp"sv,
        "cos"sv,
        "parabola"sv,
        "phaser"sv
    };
};

// ramp, cos, parabola
struct Filter_Resonance : FloatParam<Filter_Resonance> {
    static constexpr int kArgIdx = 5;
    static constexpr auto kName = "resonance"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 20.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 1;
    static constexpr auto kStuff = "dB"sv;
};

struct Filter_ResonanceWidth : FloatParam<Filter_ResonanceWidth> {
    static constexpr int kArgIdx = 6;
    static constexpr auto kName = "reso.w"sv;
    static constexpr float kMin = 0.1f;
    static constexpr float kMax = 24.0f;
    static constexpr float kDefault = 12.0f;
    static constexpr int kTextPrecision = 1;
    static constexpr auto kStuff = "st"sv;
};

// phaser reso
struct Filter_PhaserResoCycles : FloatParam<Filter_PhaserResoCycles> {
    static constexpr int kArgIdx = 7;
    static constexpr auto kName = "cycles"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 80.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 1;
};

struct Reso_PhaserResoWidth : FloatParam<Reso_PhaserResoWidth> {
    static constexpr int kArgIdx = 6;
    static constexpr auto kName = "reso.w"sv;
    static constexpr float kMin = 0.1f;
    static constexpr float kMax = 120.0f;
    static constexpr float kDefault = 12.0f;
    static constexpr int kTextPrecision = 1;
    static constexpr auto kStuff = "st"sv;
};

// ============================================================
// comb filter
// ============================================================
struct Filter_KeyTracking : FloatParam<Filter_KeyTracking> {
    static constexpr int kArgIdx = 3;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 1;
    static constexpr auto kName = "key_track"sv;
};

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

// ============================================================
// vowel
// ============================================================
struct VowelFilter_Formant : FloatParam<VowelFilter_Formant> {
    static constexpr int kArgIdx = 0;
    static constexpr float kMin = -12.0f;
    static constexpr float kMax = 12.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 1;
    static constexpr auto kName = "formant"sv;
    static constexpr auto kStuff = "st"sv;
};

struct VowelFilter_Singer : FloatChoiceParam<VowelFilter_Singer> {
    static constexpr int kArgIdx = 1;
    static constexpr auto kName = "singer"sv;
    static constexpr std::array kNames{
        "soprano"sv,
        "alto"sv,
        "countertenor"sv,
        "tenor"sv,
        "bass"sv
    };

    enum class ParamEnum {
        kSoprano = 0,
        kAlto,
        kCountertenor,
        kTenor,
        kBass,
        kNumEnums
    };
};

struct VowelFilter_Slope : FloatParam<VowelFilter_Slope> {
    static constexpr int kArgIdx = 2;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 6.0f;
    static constexpr float kDefault = 6.0f;
    static constexpr int kTextPrecision = 1;
    static constexpr auto kName = "slope"sv;
    static constexpr auto kStuff = "dB"sv;
};

struct VowelFilter_Resonance : FloatParam<VowelFilter_Resonance> {
    static constexpr int kArgIdx = 3;
    static constexpr float kMin = 1.0f;
    static constexpr float kMax = 5.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 1;
    static constexpr auto kName = "resonance"sv;
};

struct VowelFilter_Select : FloatChoiceParam<VowelFilter_Select> {
    static constexpr int kArgIdx = 4;
    static constexpr auto kName = "resonance"sv;
    static constexpr std::array kNames{
        "a"sv,
        "e"sv,
        "i"sv,
        "o"sv,
        "u"sv
    };

    enum class ParamEnum {
        kA = 0,
        kE,
        kI,
        kO,
        kU,
        kNumEnums
    };
};
}