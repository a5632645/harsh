#pragma once
#include <cmath>
#include <numbers>

namespace mana {
struct Signal {
    /**
     * @param cycles >=0
     * @param ratio 0..1
     * @param phase 0..1
     * @return 0..1
    */
    inline static float phasor(float cycles, float ratio, float phase) noexcept {
        return std::fmod(cycles * ratio + phase, 1.0F);
    }

    /**
     * @param cycles any
     * @param phase 0..1
     * @param p 0..1
     * @return -1..1
    */
    inline static float sine(float cycles, float ratio, float phase) noexcept {
        return std::sin(std::numbers::pi_v<float> *2.0F * (cycles * ratio + phase));
    }

    /**
     * @param x 0..1
     * @return -1..1
    */
    inline static float sine(float x) {
        return std::sin(std::numbers::pi_v<float> * 2.0F * x);
    }

    /**
     * @param cycles >=0
     * @param phase 0..1
     * @param p 0..1
     * @return -1..1
    */
    inline static float square(float cycles, float phase, float p) noexcept {
        auto pp = phasor(cycles, phase, p);
        return pp < 0.5F ? 1.0F : -1.0F;
    }

    /**
     * @param x 0..1
     * @return -1..1
    */
    inline static float square(float x) {
        return x < 0.5F ? 1.0F : -1.0F;
    }

    /**
     * @param cycles >=0
     * @param ratio 0..1
     * @param phase 0..1
     * @param blend 0..1
     * @return -1..1
    */
    inline static float square_blend(float cycles, float ratio, float phase, float blend) noexcept {
        auto p = phasor(cycles, ratio, phase);
        return p < blend ? 1.0F : -1.0F;
    }

    /**
     * @param x 0..1
     * @param blend 0..1
     * @return -1..1
    */
    inline static float square_blend(float x, float blend) {
        return x < blend ? 1.0F : -1.0F;
    }

    /**
     * @param cycles >=0
     * @param phase 0..1
     * @param p 0..1
     * @return -1..1
    */
    inline static float triangle(float cycles, float phase, float p) noexcept {
        auto pp = phasor(cycles, phase, p);
        return 4.0F * std::abs(pp - 0.5F) - 1.0F;
    }

    /**
     * @param x 0..1
     * @return -1..1
    */
    inline static float triangle(float x) {
        return 4.0F * std::abs(x - 0.5F) - 1.0F;
    }

    /**
     * @param cycles >=0
     * @param ratio 0..1
     * @param phase 0..1
     * @param blend 0..1
     * @return -1..1
    */
    inline static float triangle_blend(float cycle, float ratio, float phase, float blend) noexcept {
        auto f = phasor(cycle, ratio, phase);
        auto o = 1.0F;
        if (f < blend) {
            o = f / blend;
        }
        if (f > blend) {
            o = (1.0F - f) / (1.0F - blend);
        }
        return scale_up(o);
    }

    /**
     * @param x 0..1
     * @param blend 0..1
     * @return -1..1
    */
    inline static float triangle_blend(float x, float blend) {
        auto o = 1.0F;
        if (x < blend) {
            o = x / blend;
        }
        if (x > blend) {
            o = (1.0F - x) / (1.0F - blend);
        }
        return scale_up(o);
    }

    /**
     * @param cycles >=0
     * @param phase 0..1
     * @param p 0..1
     * @return -1..1
    */
    inline static float sawtooth(float cycles, float phase, float p) noexcept {
        auto pp = phasor(cycles, phase, p);
        return pp * 2.0F - 1.0F;
    }

    /**
     * @param x -1..1
     * @return 0..1
    */
    inline static float scale_down(float x) noexcept {
        return 0.5F + 0.5F * x;
    }

    /**
     * @param x 0..1
     * @return -1..1
    */
    inline static float scale_up(float x) noexcept {
        return 2.0F * x - 1.0F;
    }
};
}