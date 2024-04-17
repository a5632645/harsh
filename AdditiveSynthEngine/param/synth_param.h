#pragma once

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
        float arg0;
        float arg1;
        float arg2;
        float arg3;
    } timber;

    // dissonance
    struct {
        bool is_enable;
        int dissonance_type;
        float arg0;
        float arg1;
    } dissonance;
};
}