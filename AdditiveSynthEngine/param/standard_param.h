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
}