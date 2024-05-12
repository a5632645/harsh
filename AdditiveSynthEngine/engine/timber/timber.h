#pragma once

#include "timber_gen.h"
#include "engine/partials.h"

namespace mana {
class Timber {
public:
    Timber();

    void Init(float sample_rate, float update_rate);
    void Process(Partials& partials);
    void PrepareParams(OscillorParams& params);
    void OnUpdateTick();
    void OnNoteOn(int note);
    void OnNoteOff();
private:
    TimberFrame osc1_timber_{};
    TimberFrame osc2_timber_{};
    TimberGen osc1_;
    TimberGen osc2_;

    bool is_work_{};
    PolyModuFloatParameter* arg_osc2_timber_shift_{};
    PolyModuFloatParameter* arg_osc2_beating_{};
    PolyModuFloatParameter* arg_osc1_gain_{};
    PolyModuFloatParameter* arg_osc2_gain_{};
    float osc2_timber_shift_{};
    float osc2_beating_{};
    float osc1_gain_{};
    float osc2_gain_{};
    float beating_phase_{};
    float inv_update_rate_{};
};
}