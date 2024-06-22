#pragma once

#include <array>
#include <complex>
#include "EngineConfig.h"

namespace mana {
struct Partials {
    static constexpr auto kMaxFreq = 20'000.0f;

    bool update_phase{};   // should update sinebank current phase
    float base_frequency{};// base frequency in hz
    float base_pitch{};    // base pitch in semitones

    std::array<float, kNumPartials> gains{};  // partial gain
    std::array<float, kNumPartials> freqs{};  // partial frequency in hz
    std::array<float, kNumPartials> pitches{};// partial pitch in semitones
    std::array<float, kNumPartials> ratios{}; // partial frequency ratio
    std::array<std::complex<float>, kNumPartials> phases{}; // partial phase
};
}