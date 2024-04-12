#include <chrono>
#include <cmath>
#include "utli/DebugMarco.h"

namespace mana {
class ExpSmoothFilter {
public:
    void init_dsp(float sample_rate) {
        DSP_INIT;

        m_sample_rate_div_1000 = sample_rate / 1000.0F;
    }

    void set_attack(std::chrono::milliseconds time) {
        set_attack(static_cast<float>(time.count()));
    }

    void set_attack(float const milliseconds) {
        DSP_INIT_ASSERT;

        m_coef_a = std::exp(-1.0F / (m_sample_rate_div_1000 * milliseconds));
    }

    void set_attack_seconds(float const seconds) {
        set_attack(seconds * 1000.0F);
    }

    void set_release(std::chrono::milliseconds time) {
        set_release(static_cast<float>(time.count()));
    }

    void set_release_seconds(float const seconds) {
        set_release(seconds * 1000.0F);
    }

    void set_release(float const milliseconds) {
        DSP_INIT_ASSERT;

        m_coef_r = std::exp(-1.0F / (m_sample_rate_div_1000 * milliseconds));
    }

    void smooth_ar(float& now, float const target) const {
        DSP_INIT_ASSERT;

        if (target > now) {
            now = m_coef_a * now + (1.0F - m_coef_a) * target;
        }
        else {
            now = m_coef_r * now + (1.0F - m_coef_r) * target;
        }
    }

    void smooth_a(float& now, float const target) const {
        DSP_INIT_ASSERT;

        now = m_coef_a * now + (1.0F - m_coef_a) * target;
    }
private:
    float m_coef_a{};
    float m_coef_r{};
    float m_sample_rate_div_1000{};

    DSP_INIT_DEFINE;
};
}