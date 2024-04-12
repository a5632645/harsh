#pragma once
#include <array>
#include "EngineConfig.h"

namespace mana {
struct Partials {
    bool update_phase{};
    float base_frequency{};
    
    std::array<float, kNumPartials> gains{};
    std::array<float, kNumPartials> freqs{};
    std::array<float, kNumPartials> phases{};
};
}