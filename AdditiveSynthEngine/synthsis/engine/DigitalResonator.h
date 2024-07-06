#pragma once

#include <cmath>
#include <concepts>
#include <numbers>

namespace mana {
template<std::floating_point T>
struct DigitalResonator {
    T sin_reg0_{};
    T sin_reg1_{};
    T cos_reg0_{};
    T cos_reg1_{};
    T c_{};
    T freq_{};

    DigitalResonator() {
        Init(T{});
    }

    void Init(T normalize_phase, T normalize_freq = T{}) noexcept {
        const auto phase = normalize_phase * 2 * std::numbers::pi_v<T>;
        const auto freq = normalize_freq * std::numbers::pi_v<T>;
        sin_reg0_ = std::sin(phase);
        sin_reg1_ = std::sin(phase + freq);
        cos_reg0_ = std::cos(phase);
        cos_reg1_ = std::cos(phase + freq);
        c_ = 2 * std::cos(freq);
        freq_ = normalize_freq;
    }

    void SetRunTimeFreq(T normalize_freq) noexcept {
        freq_ = normalize_freq;
        T f_cos = std::cos(normalize_freq * std::numbers::pi_v<T>);
        T f_sin = std::sin(normalize_freq * std::numbers::pi_v<T>);
        sin_reg1_ = sin_reg0_ * f_cos + cos_reg0_ * f_sin;
        cos_reg1_ = cos_reg0_ * f_cos - sin_reg0_ * f_sin;
        c_ = 2 * f_cos;
    }

    T Tick() noexcept {
        T out = sin_reg0_;

        T e = c_ * sin_reg1_ - sin_reg0_;
        sin_reg0_ = sin_reg1_;
        sin_reg1_ = e;
        T e_cos = c_ * cos_reg1_ - cos_reg0_;
        cos_reg0_ = cos_reg1_;
        cos_reg1_ = e_cos;

        return out;
    }

    T sin() const {
        return sin_reg0_;
    }

    T cos() const {
        return cos_reg0_;
    }
};
}