#pragma once

#include "engine/partials.h"
#include "timber_frame.h"
#include "engine/forward_decalre.h"
#include "engine/poly_param.h"

namespace mana {
class TimberGen;
}

namespace mana {
class Timber {
public:
    static void AddSynthParam(SynthParams& s, ParamCreator& c);
    Timber();
    ~Timber();

    Timber(const Timber&) = delete;
    Timber& operator=(const Timber&) = delete;
    Timber(Timber&&) noexcept;
    Timber& operator=(Timber&&) noexcept;

    void Init(float sample_rate, float update_rate);
    void Process(Partials& partials);
    void PrepareParams(ModulableParams& params);
    void OnUpdateTick();
    void OnNoteOn(int note);
    void OnNoteOff();
private:
    TimberFrame osc1_timber_{};
    TimberFrame osc2_timber_{};
    std::unique_ptr<TimberGen> osc1_;
    std::unique_ptr<TimberGen> osc2_;

    ModuFloatParameter* arg_osc2_timber_shift_{};
    ModuFloatParameter* arg_osc2_beating_{};
    ModuFloatParameter* arg_osc1_gain_{};
    ModuFloatParameter* arg_osc2_gain_{};
    float osc2_timber_shift_{};
    float osc2_beating_{};
    float osc1_gain_{};
    float osc2_gain_{};
    float beating_phase_{};
    float inv_update_rate_{};
};
}