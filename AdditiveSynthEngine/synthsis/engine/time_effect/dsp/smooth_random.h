#pragma once

#include <random>

namespace mana::dsp {
template<std::floating_point T>
class SmoothRandom {
public:
    SmoothRandom(T min, T max)
        : dist_(min, max) {
        curr_random_ = dist_(random_);
    }

    void Init(T sample_rate) {
        sample_rate_ = sample_rate;
    }

    void SetRate(T rate) {
        rate_ = rate;
        osc_phase_inc_ = rate_ / sample_rate_;
    }

    T Next() {
        if (OscAdvance()) {
            last_random_ = curr_random_;
            curr_random_ = dist_(random_);
        }
        return std::lerp(last_random_, curr_random_, osc_phase_);
    }
private:
    bool OscAdvance() {
        osc_phase_ += osc_phase_inc_;
        if (osc_phase_ > 1.0f) {
            osc_phase_ -= 1.0f;
            return true;
        }
        return false;
    }

    std::default_random_engine random_;
    std::uniform_real_distribution<T> dist_;
    T sample_rate_{};
    T rate_{};
    T osc_phase_{};
    T osc_phase_inc_{};
    T last_random_{};
    T curr_random_{};
};
}