#pragma once

#include <array>
#include <complex>
#include "EngineConfig.h"

namespace mana {
struct Partials {
    bool update_phase{};
    float base_frequency{};
    float base_pitch{};
    int stop_index{};

    std::array<float, kNumPartials> gains{};
    std::array<float, kNumPartials> freqs{};
    std::array<float, kNumPartials> pitches{};
    std::array<std::complex<float>, kNumPartials> phases{};
};
}