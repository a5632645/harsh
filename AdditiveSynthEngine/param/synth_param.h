#pragma once

#include <array>

namespace mana {
struct SynthParam {
    struct {
        float pitch_bend;
    } standard;

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
    std::array<EffectParams, 3 > effects;
};
}