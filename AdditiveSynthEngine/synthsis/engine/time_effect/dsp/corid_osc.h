#pragma once

#include <complex>
#include <numbers>

namespace mana::dsp {
template<std::floating_point T>
class CoridOscillor {
public:
    CoridOscillor() {
        ResetGain();
    }

    void Init(T sample_rate) {
        inv_sample_rate_ = 1.0f / sample_rate;
    }

    void SetPhase(T phase) {
        phase_ = std::polar(static_cast<T>(1.0f), phase);
    }

    void SetFreq(T freq) {
        phase_inc_ = std::polar(static_cast<T>(1.0f), freq * inv_sample_rate_ * 2.0f * std::numbers::pi_v<T>);
    }

    void TickNext() {
        phase_ *= phase_inc_;
    }

    T Sine() const {
        return phase_.imag();
    }

    T Cosine() const {
        return phase_.real();
    }

    void ResetGain() {
        SetPhase(std::arg(phase_));
    }
private:
    T inv_sample_rate_{};
    std::complex<T> phase_;
    std::complex<T> phase_inc_;
};
}