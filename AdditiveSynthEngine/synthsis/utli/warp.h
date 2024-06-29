#pragma once

#include <cmath>
#include <gcem.hpp>

namespace mana::utli::warp {
static float ParabolaWarp(float x, float w) {
    return (w + 1.0f - w * std::abs(x)) * x;
}

template<std::floating_point T>
inline static constexpr T ExpWarp(T x, T e) {
    if (std::is_constant_evaluated()) {
        if (gcem::abs(e) < 1e-3) // almost line
            return x;

        auto down = gcem::exp(e) - 1.0f;
        auto up = gcem::exp(e * x) - 1.0f;
        return up / down;
    }
    else {
        if (std::abs(e) < 1e-3) // almost line
            return x;

        auto down = std::exp(e) - 1.0f;
        auto up = std::exp(e * x) - 1.0f;
        return up / down;
    }
}

static constexpr decltype(auto) AtNormalizeIndex(std::ranges::range auto& arr, float nor_idx) {
    auto size = std::ranges::size(arr);
    auto idx = static_cast<size_t>((size - 0.01f) * nor_idx);
    return (arr[idx]);
}
}