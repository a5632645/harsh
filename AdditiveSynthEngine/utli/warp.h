#pragma once

#include <cmath>

namespace mana::utli::warp {
static float ParabolaWarp(float x, float w) {
    return (w + 1.0f - w * std::abs(x)) * x;
}

static constexpr auto GetRangeByNorIdx(std::ranges::input_range auto& arr, float nor_idx) {
    auto size = std::ranges::size(arr) - 1;
    auto idx = static_cast<size_t>(size * nor_idx);
    return arr[idx];
}
}