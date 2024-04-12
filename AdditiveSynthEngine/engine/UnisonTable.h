#pragma once
#include <array>

namespace mana {
template<size_t NVOICE, size_t SIZE>
static constexpr auto MakeOneLineTable() {
    std::array<float, SIZE> res{};

    if constexpr (NVOICE <= 1) {
        return res;
    }

    /*if constexpr (NVOICE > 1) {
        auto interval = 2.0F / (NVOICE - 1);
        for (size_t i = 0; i < NVOICE; i++) {
            res[i] = -1.0F + interval * i;
        }
    }*/

    for (size_t i = 0; i < NVOICE; ++i) {
        res[i] = -1.0F + 2.0F * (i + 1) / (NVOICE + 1);
    }
    
    return res;
}

template<size_t... N>
static constexpr auto MakeUnisonTable(std::index_sequence<N...>) {
    constexpr auto n = sizeof...(N);
    return std::array<std::array<float, n>, n>{ MakeOneLineTable<N, n>()... };
}

template<size_t N>
static constexpr auto UNISON_TABLE_V = MakeUnisonTable(std::make_index_sequence<N + 1>());
}