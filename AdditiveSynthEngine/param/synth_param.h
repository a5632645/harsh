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

    struct {
        int timber_type;
        std::array<float, 4> args;
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