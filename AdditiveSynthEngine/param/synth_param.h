#pragma once

#include <array>

namespace mana {
struct SynthParam {
    // standard
    struct {
        float pitch_bend;
        struct {
            float phase_type;
            float phase_random;
        } phase;
    } standard;

    // timber
    struct {
        int timber_type;
        std::array<float, 4> args;
    } timber;

    // dissonance
    struct {
        bool is_enable;
        int dissonance_type;
        float arg0;
        float arg1;
    } dissonance;

    // filter
    struct {
        bool is_enable;
        int filter_type;
        float arg0;
        float arg1;
        float arg2;
        float arg3;
        float arg4;
        float arg5;
    } filter;

    // effect
    struct EffectParams {
        bool is_enable;
        int effect_type;
        std::array<float, 6> args;
    };
    std::array<EffectParams, 3 > effects;
};
}