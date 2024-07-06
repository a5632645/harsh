#pragma once

#include <concepts>
#include <numbers>

namespace mana::dsp {
template<std::floating_point T>
class StateVarFilter {
public:
    void SetNormalizeCutoff(T cutoff) { wct_ = cutoff; }
    void Process(T input) {
        auto a0 = input - lp_ - std::numbers::sqrt2_v<T> *z0_;
        hp_ = a0;
        auto a1 = a0 * wct_ + z0_;
        z0_ = a1;
        bp_ = a1;
        auto a2 = z1_ + wct_ * z0_;
        lp_ = a2;
        z1_ = a2;
    }
    T GetLowpass() const { return lp_; }
    T GetBandpass() const { return bp_; }
    T GetHighpass() const { return hp_; }
private:
    T wct_{};
    T z0_{};
    T z1_{};
    T hp_{};
    T bp_{};
    T lp_{};
};
}