#pragma once

#include <array>
#include <random>
#include <cassert>
#include <cmath>

namespace mana::utli {
template<size_t kSize>
class NoiseGenerator {
public:
    void Init(float sample_rate) {
        sample_rate_ = sample_rate;
    }

    void CrTick(int skip) {
        lfo_phase_ += skip * phase_sr_inc_;

        if (lfo_phase_ > 1.0f) {
            lfo_phase_ -= static_cast<float>(static_cast<int>(lfo_phase_));
            buffer0_ = buffer1_;
            FillNoise(buffer1_);
        }
    }

    void FillNoise(std::ranges::range auto& rng) {
        for (auto& s : rng) {
            auto v = static_cast<float>(random_()) / static_cast<float>(std::random_device::max());
            s = 2.0f * v - 1.0f;
        }
    }

    void SetLfoSpeed(float rate) {
        assert(rate >= 0.0f);
        assert(sample_rate_ != 0.0f);

        phase_sr_inc_ = rate / sample_rate_;
    }

    float GetNoise(int bin) {
        return GetNoise(bin, lfo_phase_);
    }

    float GetNoise(int bin, float lerp_val) {
        auto val0 = buffer0_[bin];
        auto val1 = buffer1_[bin];
        return std::lerp(val0, val1, lerp_val);
    }
private:
    float phase_sr_inc_{};
    float lfo_phase_{};
    float sample_rate_{};

    std::array<float, kSize> buffer0_{};
    std::array<float, kSize> buffer1_{};
    std::random_device random_;
};
}