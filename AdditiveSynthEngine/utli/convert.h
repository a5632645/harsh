#pragma once

#include <gcem.hpp>
#include <cmath>

namespace mana::utli {
namespace cp {
static consteval float DbToGain(float db) {
    return gcem::exp(0.11512925464970228420089957273422f * db);
}

static consteval float DbToGain(float db, float min_db) {
    if (db <= min_db) {
        return 0.0f;
    }
    return gcem::exp(0.11512925464970228420089957273422f * db);
}

static consteval float PitchToFreq(float pitch) {
    return gcem::exp(pitch * 0.05776226504666210911810267678818f) * 8.1758f;
}

static consteval float Calc1stSmoothFilterCoeff(float time_second, float update_rate) {
    if (time_second < 1.0f / 1000.0f) {
        return 0.0f;
    }
    return gcem::exp(-1.0f / (update_rate * time_second));
}
} // complite time

static float DbToGain(float db) {
    return std::exp(0.11512925464970228420089957273422f * db);
}

static float DbToGain(float db, float min_db) {
    if (db <= min_db) {
        return 0.0f;
    }
    return std::exp(0.11512925464970228420089957273422f * db);
}

template<float kMinDb>
static float GainToDb(float gain) {
    constexpr auto min_gain = cp::DbToGain(kMinDb);
    if (gain <= min_gain) {
        return kMinDb;
    }
    return 20.0f * log10(gain);
}

static float PitchToFreq(float pitch) {
    return std::exp2(pitch / 12.0f) * 8.1758f;
}

static float Calc1stSmoothFilterCoeff(float time_second, float update_rate) {
    if (time_second < 1.0f / 1000.0f) {
        return 0.0f;
    }
    return std::exp(-1.0f / (update_rate * time_second));
}
}