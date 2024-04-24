#pragma once

#include <array>
#include "param/param.h"

namespace mana::param {
struct EffectType {
    enum class EffectTypeEnum {
        kOctaver = 0,
        kReverb,
        kChorus,
        kNumEnums
    };

    static constexpr auto kName = "type"sv;
    static constexpr std::array kNames{
            "octaver"sv,
            "reverb"sv,
            "chorus"sv
    };
};

// =========================================================
// octaver
// =========================================================
struct Octaver_Amount { // arg0
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "amount"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 1.0f;
    static constexpr auto kTextPrecision = 2;
};

struct Octaver_Width { // arg1
    static constexpr int kArgIdx = 1;
    static constexpr auto kName = "width"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 8.0f;
    static constexpr auto kDefault = 1.0f;
    static constexpr auto kTextPrecision = 1;
};

struct Octaver_Decay { //arg2
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
struct Reverb_Amount { // arg0
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "amount"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 1.0f;
    static constexpr auto kTextPrecision = 2;
};

struct Reverb_Decay { // arg1
    static constexpr int kArgIdx = 1;
    static constexpr auto kName = "decay"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 5000.0f;
    static constexpr auto kDefault = 1000.0f;
    static constexpr auto kTextPrecision = 0;
};

struct Reverb_Attack { //arg2
    static constexpr int kArgIdx = 2;
    static constexpr auto kName = "attack"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 5000.0f;
    static constexpr auto kDefault = 1000.0f;
    static constexpr auto kTextPrecision = 0;
};

struct Reverb_Damp { //arg3
    static constexpr int kArgIdx = 3;
    static constexpr auto kName = "damp"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 1.0f;
    static constexpr auto kTextPrecision = 2;
};

// =========================================================
// down sample
// =========================================================
struct Chorus_Amount {
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "amount"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 1.0f;
    static constexpr auto kTextPrecision = 2;
};

struct Chorus_Depth {
    static constexpr int kArgIdx = 1;
    static constexpr auto kName = "depth"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 10.0f;
    static constexpr auto kDefault = 10.0f;
    static constexpr auto kTextPrecision = 1;
    static constexpr auto kStuff = "ms";
};

struct Chorus_Offset {
    static constexpr int kArgIdx = 2;
    static constexpr auto kName = "offset"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 10.0f;
    static constexpr auto kDefault = 10.0f;
    static constexpr auto kTextPrecision = 1;
    static constexpr auto kStuff = "ms";
};

struct Chorus_Speed {
    static constexpr int kArgIdx = 3;
    static constexpr auto kName = "speed"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 0.0f;
    static constexpr auto kTextPrecision = 1;
    static constexpr auto kStuff = "hz"sv;
};
}