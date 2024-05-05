#pragma once

#include <array>

namespace mana {
struct SynthParam {
    struct {
        float pitch_bend;
        float output_gain;
    } standard;

    struct {
        int phase_type;
        std::array<float, 2> args;
    } phase;

    struct OscParam {
        int timber_type;
        std::array<float, 4> args;
    };
    struct {
        std::array<OscParam, 2> osc_args;
        float osc2_shift;
        float osc2_beating;
        float osc1_gain;
        float osc2_gain;
    } timber;

    struct {
        bool is_enable;
        int dissonance_type;
        std::array<float, 2> args;
    } dissonance;

    struct {
        bool is_enable;
        std::array<float, 7> args;
    } resynthsis;

    struct {
        bool is_enable;
        int filter_type;
        std::array<float, 6> args;
    } filter;

    struct EffectParams {
        bool is_enable;
        int effect_type;
        std::array<float, 6> args;
    };
    std::array<EffectParams, 5 > effects;
};
}