#pragma once

#include <array>
#include <utility>
#include <ranges>
#include <algorithm>
#include <numeric>
#include <gcem.hpp>

template<size_t N, size_t... Is>
constexpr std::array<float, N> makeArray(std::index_sequence<Is...>) {
    return { (Is + 1.0F)... };
};

template<size_t N, typename Func>
constexpr std::array<float, N> MakeNormalizeTable(Func func) {
    std::array<float, N> res;
    for (size_t i = 0; i < N; ++i) {
        auto pos = static_cast<float>(i) / static_cast<float>(N);
        res[i] = func(pos);
    }
    return res;
}

template<size_t N, typename Fun>
constexpr std::array<float, N> makeHarmonicArray(Fun fun) {
    auto a = makeArray<N>(std::make_index_sequence<N>());
    for (size_t i = 0; i < N; i++) {
        a[i] = fun(a[i]);
    }
    return a;
};

template<typename T, size_t N>
static constexpr auto NormalizeTable(const std::array<T, N>& table) {
    std::array<T, N> out;
    auto max_val = *std::ranges::max_element(table);
    auto level_gain = static_cast<T>(1) / max_val;
    std::ranges::transform(table, out.begin(), [level_gain](auto v) {return v * level_gain; });
    return out;
};

template<typename T, size_t N>
static constexpr auto NormalizeTableByPower(const std::array<T, N>& table) {
    auto power_sum = std::accumulate(table.cbegin(), table.cend(), T{}, [](T sum, T curr) {
        return sum + curr * curr;
    });
    auto root_power_sum = gcem::sqrt(power_sum * 2 / N);
    auto gain_level = static_cast<T>(1) / root_power_sum;

    std::array<T, N> out{};
    std::ranges::transform(table, out.begin(), [gain_level](auto v) {
        return v * gain_level;
    });

    return out;
};

template<size_t N>
struct VolumeTable {
    inline static constexpr auto PULSE_TABLE = makeHarmonicArray<N>([](float x) {
        if (static_cast<int>(x) & 1) {
            return 1.0F / x;
        }
        else {
            return 0.0F;
        }
    });
    inline static constexpr auto ALL_TABLE = makeHarmonicArray<N>([](float x) { return 0.05F; });
    inline static constexpr auto SAW_TABLE = makeHarmonicArray<N>([](float x) {return 1.0F / x; });
    inline static constexpr auto SAW_DOUBLE_TABLE = makeHarmonicArray<N>([](float x) {
        if (static_cast<int>(x) & 1) {
            return 0.0F;
        }
        else {
            return 1.0F / x;
        }
    });
};