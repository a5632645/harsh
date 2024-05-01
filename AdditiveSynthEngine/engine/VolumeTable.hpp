#pragma once
#include <array>
#include <utility>


template<size_t N,size_t... Is>
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
