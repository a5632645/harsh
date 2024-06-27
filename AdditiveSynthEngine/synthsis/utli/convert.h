#pragma once

#include <gcem.hpp>
#include <cmath>

namespace mana::utli {
template<std::floating_point T>
inline static constexpr T Exp2(T x) {
    if constexpr (std::is_constant_evaluated()) {
        return gcem::exp(0.69314718055994530941723212145818f * x);
    }
    else {
        return std::exp2(x);
    }
}

template<std::floating_point T>
inline static constexpr T DbToGain(T db) {
    if constexpr (std::is_constant_evaluated()) {
        return gcem::exp(0.11512925464970228420089957273422f * db);
    }
    else {
        return std::exp(0.11512925464970228420089957273422f * db);
    }
}

template<std::floating_point T>
inline static constexpr T DbToGain(T db, T min_db) {
    if (db <= min_db) {
        return 0.0f;
    }
    return DbToGain(db);
}

template<float kMinDb = -300.0f, std::floating_point T>
inline static constexpr T GainToDb(T gain) {
    constexpr auto min_gain = DbToGain(kMinDb);
    if (gain < min_gain) {
        return kMinDb;
    }
    if constexpr (std::is_constant_evaluated()) {
        return 20.0f * gcem::log10(gain);
    }
    else {
        return 20.0f * std::log10(gain);
    }
}

template<std::floating_point T>
inline static constexpr T PitchToFreq(T pitch) {
    if constexpr (std::is_constant_evaluated()) {
        return gcem::exp(pitch * 0.05776226504666210911810267678818f) * 8.1758f;
    }
    else {
        return std::exp2(pitch / 12.0f) * 8.1758f;
    }
}

template<std::floating_point T>
inline static constexpr T FreqToPitch(T freq) {
    if constexpr (std::is_constant_evaluated()) {
        return gcem::log(freq / 8.1758f) / 0.05776226504666210911810267678818f;
    }
    else {
        return std::log(freq / 8.1758f) / 0.05776226504666210911810267678818f;
    }
}

template<std::floating_point T>
inline static constexpr T Calc1stSmoothFilterCoeff(T time_second, T update_rate) {
    if constexpr (std::is_constant_evaluated()) {
        return gcem::exp(-1.0f / (update_rate * time_second));
    }
    else {
        return std::exp(-1.0f / (update_rate * time_second));
    }
}

template<std::floating_point T>
inline static constexpr T Calc1stSmoothFilterCoeffByDecayRate(T db_per_second, T update_rate) {
    if constexpr (std::is_constant_evaluated()) {
        return gcem::exp(-0.11512925464970228420089957273422f * db_per_second / update_rate);
    }
    else {
        return std::exp(-0.11512925464970228420089957273422f * db_per_second / update_rate);
    }
}

template<std::floating_point T>
inline static constexpr T RatioToPitch(T ratio, T base_pitch) {
    ratio = std::max(ratio, 0.0001f);
    if constexpr (std::is_constant_evaluated()) {
        return base_pitch + 12.0f * gcem::log2(ratio);
    }
    else {
        return base_pitch + 12.0f * std::log2(ratio);
    }
}
}