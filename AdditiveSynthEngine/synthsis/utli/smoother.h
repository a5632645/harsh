#pragma once

#include "convert.h"

namespace mana::utli {
class Smoother {
public:
    constexpr Smoother() = default;
    constexpr Smoother(float init) : target_(init), current_(init) {}

    constexpr void SetSmooth(float sample_rate, float millseonds = 50.0f) {
        a_ = Calc1stSmoothFilterCoeff(millseonds / 1000.0f, sample_rate);
        b_ = 1.0f - a_;
    }
    constexpr void SetTarget(float target) { target_ = target; }
    constexpr void ForceToTarget(float target) {
        current_ = target;
        target_ = target;
    }
    constexpr float GetTarget() const { return target_; }
    constexpr float GetCurrent() const { return current_; }
    constexpr float TickNext() {
        auto e = current_;
        current_ = a_ * current_ + b_ * target_;
        return e;
    }
private:
    float a_{};
    float b_{};
    float target_{};
    float current_{};
};
}