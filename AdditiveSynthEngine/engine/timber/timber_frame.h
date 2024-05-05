#pragma once

#include <array>
#include <cmath>
#include <numbers>
#include "engine/EngineConfig.h"

namespace mana {
struct TimberFrame {
    float base_freq;
    std::array<float, kNumPartials> gains;
};
}