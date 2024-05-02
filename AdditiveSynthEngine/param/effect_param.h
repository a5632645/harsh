#pragma once

#include "param/param.h"

namespace mana::param {
struct EffectType : IntChoiceParam<EffectType> {
    enum class ParamEnum {
        kOctaver = 0,
        kReverb,
        kChorus,
        kPhaser,
        kScramble,
        kBlur,
        kNumEnums
    };

    static constexpr auto kName = "type"sv;
    static constexpr std::array kNames{
            "octaver"sv,
            "reverb"sv,
            "chorus"sv,
            "phaser"sv,
            "scramble"sv,
            "blur"sv,
    };
};

// =========================================================
// octaver
// =========================================================
struct Octaver_Amount : FloatParam<Octaver_Amount> {
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "amount"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 1.0f;
    static constexpr auto kTextPrecision = 2;
};

struct Octaver_Width : FloatParam<Octaver_Width> {
    static constexpr int kArgIdx = 1;
    static constexpr auto kName = "width"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 8.0f;
    static constexpr auto kDefault = 1.0f;
    static constexpr auto kTextPrecision = 1;
};

struct Octaver_Decay : FloatParam<Octaver_Decay> {
    static constexpr int kArgIdx = 2;
    static constexpr auto kName = "decay"sv;
    static constexpr auto kMin = -6.0f;
    static constexpr auto kMax = 0.0f;
    static constexpr auto kDefault = -6.0f;
    static constexpr auto kTextPrecision = 2;
    static constexpr auto kStuff = "dB/oct";
};

// =========================================================
// reverb
// =========================================================
struct Reverb_Amount : FloatParam<Reverb_Amount> {
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "amount"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 1.0f;
    static constexpr auto kTextPrecision = 2;
};

struct Reverb_Decay : FloatParam<Reverb_Decay> {
    static constexpr int kArgIdx = 1;
    static constexpr auto kName = "decay"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 5000.0f;
    static constexpr auto kDefault = 1000.0f;
    static constexpr auto kTextPrecision = 0;
};

struct Reverb_Attack : FloatParam<Reverb_Attack> {
    static constexpr int kArgIdx = 2;
    static constexpr auto kName = "attack"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 5000.0f;
    static constexpr auto kDefault = 1000.0f;
    static constexpr auto kTextPrecision = 0;
};

struct Reverb_Damp : FloatParam<Reverb_Damp> {
    static constexpr int kArgIdx = 3;
    static constexpr auto kName = "damp"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 1.0f;
    static constexpr auto kTextPrecision = 2;
};

// =========================================================
// chorus
// =========================================================
struct Chorus_Amount : FloatParam<Chorus_Amount> {
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "amount"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 1.0f;
    static constexpr auto kTextPrecision = 2;
};

struct Chorus_Depth : FloatParam<Chorus_Depth> {
    static constexpr int kArgIdx = 1;
    static constexpr auto kName = "depth"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 10.0f;
    static constexpr auto kDefault = 10.0f;
    static constexpr auto kTextPrecision = 1;
    static constexpr auto kStuff = "ms";
};

struct Chorus_Offset : FloatParam<Chorus_Offset> {
    static constexpr int kArgIdx = 2;
    static constexpr auto kName = "offset"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 10.0f;
    static constexpr auto kDefault = 10.0f;
    static constexpr auto kTextPrecision = 1;
    static constexpr auto kStuff = "ms";
};

struct Chorus_Speed : FloatParam<Chorus_Speed> {
    static constexpr int kArgIdx = 3;
    static constexpr auto kName = "speed"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 0.0f;
    static constexpr auto kTextPrecision = 1;
    static constexpr auto kStuff = "hz"sv;
};

// =========================================================
// phaser
// =========================================================
struct Phaser_Cycles : FloatParam<Phaser_Cycles> {
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "cycles"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 0.0f;
    static constexpr auto kTextPrecision = 1;
};

struct Phaser_Shape : FloatChoiceParam<Phaser_Shape> {
    enum class ParamEnum {
        kBox = 0,
        kParabola,
        kSine,
        kTri,
        kDeep,
        kNumEnums
    };

    static constexpr std::array kNames{
        "box"sv,
        "parabola"sv,
        "sine"sv,
        "tri"sv,
        "deep"sv
    };

    static constexpr int kArgIdx = 1;
    static constexpr auto kName = "shape"sv;
};

struct Phaser_Mix : FloatParam<Phaser_Mix> {
    static constexpr int kArgIdx = 2;
    static constexpr auto kName = "mix"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 1.0f;
    static constexpr auto kTextPrecision = 2;
};

struct Phaser_Pinch : FloatParam<Phaser_Pinch> {
    static constexpr int kArgIdx = 3;
    static constexpr auto kName = "pinch"sv;
    static constexpr auto kMin = -1.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 0.0f;
    static constexpr auto kTextPrecision = 2;
};

struct Phaser_BarberRate : FloatParam<Phaser_BarberRate> {
    static constexpr int kArgIdx = 4;
    static constexpr auto kName = "rate"sv;
    static constexpr auto kMin = -10.0f;
    static constexpr auto kMax = 10.0f;
    static constexpr auto kDefault = 0.0f;
    static constexpr auto kTextPrecision = 1;
    static constexpr auto kStuff = "hz"sv;
};

struct Phaser_Mode : FloatChoiceParam<Phaser_Mode> {
    enum class ParamEnum {
        kSemitone = 0,
        kHz,
        kHarmonic,
        kNumEnums
    };

    static constexpr std::array kNames{
        "semitone"sv,
        "hz"sv,
        "harmonic"sv
    };

    static constexpr int kArgIdx = 5;
    static constexpr auto kName = "mode"sv;
};

// =========================================================
// scramble
// =========================================================
struct Scramble_Rate : FloatParam<Scramble_Rate> {
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "rate"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 25.0f;
    static constexpr auto kDefault = 1.0f;
    static constexpr auto kTextPrecision = 1;
    static constexpr auto kStuff = "hz"sv;
};

struct Scramble_Range : FloatParam<Scramble_Range> {
    static constexpr int kArgIdx = 1;
    static constexpr auto kName = "range"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 0.2f;
    static constexpr auto kTextPrecision = 2;
};

// =========================================================
// blur
// =========================================================
struct Blur_TimeAttack : FloatParam<Blur_TimeAttack> {
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "t.att"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 2000.0f;
    static constexpr auto kDefault = 0.0f;
    static constexpr auto kTextPrecision = 1;
    static constexpr auto kStuff = "ms";
};

struct Blur_TimeRelease : FloatParam<Blur_TimeRelease> {
    static constexpr int kArgIdx = 1;
    static constexpr auto kName = "t.rel"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 2000.0f;
    static constexpr auto kDefault = 0.0f;
    static constexpr auto kTextPrecision = 1;
    static constexpr auto kStuff = "ms";
};

struct Blur_BinAttack : FloatParam<Blur_BinAttack> {
    static constexpr int kArgIdx = 2;
    static constexpr auto kName = "b.att"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 0.99f;
    static constexpr auto kDefault = 0.0f;
    static constexpr auto kTextPrecision = 3;
};

struct Blur_BinRelease : FloatParam<Blur_BinRelease> {
    static constexpr int kArgIdx = 3;
    static constexpr auto kName = "b.rel"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 0.99f;
    static constexpr auto kDefault = 1.0f;
    static constexpr auto kTextPrecision = 3;
};
}