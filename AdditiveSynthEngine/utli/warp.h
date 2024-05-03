#pragma once

#include <cmath>

namespace mana::utli::warp {
static float ParabolaWarp(float x, float w) {
    return (w + 1.0f - w * std::abs(x)) * x;
}
}