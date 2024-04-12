#pragma once
#include <cmath>

namespace mana::pc {
    static constexpr auto k_hz_of_0_semitone = 8.1757989156f;
    static constexpr auto k_min_frequency = 0.1f;

    inline static float p2f(float pitch) {
        return k_hz_of_0_semitone * std::pow(2.0F, pitch / 12.0F);
    }

    inline static float f2p(float freq) {
        freq = std::max(freq, k_min_frequency);
        return 12.0F * std::log2(freq / k_hz_of_0_semitone);
    }
}