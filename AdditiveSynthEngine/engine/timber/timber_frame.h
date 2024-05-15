#pragma once

#include <array>
#include <cmath>
#include <numbers>
#include <complex>
#include "engine/EngineConfig.h"

namespace mana {
struct TimberFrame {
    std::array<float, kNumPartials> gains;
};
}