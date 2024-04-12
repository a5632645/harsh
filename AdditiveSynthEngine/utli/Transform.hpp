#pragma once
#include <cmath>

namespace mana::smath {
static float phasor_scale(float v) {
    return v * 2.0F - 1.0F;
}

static float blend_pos_neg_1(float x, float b) {
    return (x - b) / (1.0F - b * x);
}

static float blend_01_to_01(float x, float b) {
    auto v = phasor_scale(x);
    auto vv = blend_pos_neg_1(v, b);
    return vv * 0.5F + 0.5F;
}

static float pos_neg_1_to_01(float x) {
    return x * 0.5F + 0.5F;
}

inline static float linear_interp(float x, float min, float max) {
    return (max - min) * x + min;
}

inline static float ratio(float x, float min, float max) {
    if (x < min) {
        return 0.0F;
    }
    else if (x > max) {
        return 1.0F;
    }
    else {
        return (x - min) / (max - min);
    }
}

inline static float map(float x,
                        float source_min, float source_max,
                        float target_min, float target_max) {
    auto r = ratio(x, source_min, source_max);
    return linear_interp(r, target_min, target_max);
}

inline static float mod1(float x) {
    float v;
    return std::modf(x, &v);
}
}