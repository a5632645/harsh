#pragma once

#include <cmath>

namespace mana::utli::warp {
static float ParabolaWarp(float x, float w) {
    return (w + 1.0f - w * std::abs(x)) * x;
}

static constexpr decltype(auto) AtNormalizeIndex(std::ranges::range auto& arr, float nor_idx) {
    auto size = std::ranges::size(arr);
    auto idx = static_cast<size_t>((size - 0.01f) * nor_idx);
    return (arr[idx]);
}
}