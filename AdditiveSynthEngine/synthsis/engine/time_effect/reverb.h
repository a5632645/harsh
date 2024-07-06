#pragma once

#include "timefx_base.h"
#include "engine/poly_param.h"
#include "engine/oscillor_param.h"
#include "revmodel.hpp"
#include "dsp/dsp/fdn_reverb.h"

namespace mana {
class TimeReverb : public TimeFxBase {
public:
    void Init(float sample_rate, float update_rate);
    void PrepareParams(ModulableParams& params);
    void OnUpdateTick();
    void OnNoteOn(int note) {}
    void OnNoteOff() {}
    void Process(float* pbuffer, int num) override;
private:
    BoolParameter* enable_;
    ModuFloatParameter* damp_;
    ModuFloatParameter* dry_;
    ModuFloatParameter* roomsize_;
    ModuFloatParameter* wet_;
    ModuFloatParameter* width_;
    revmodel reverb_;

    ModuFloatParameter* distribution_;
    ModuFloatParameter* separate_;
    ModuFloatParameter* feedback2_;
    ModuFloatParameter* feedback3_;
    ModuFloatParameter* freq_shift_;
    IntChoiceParameter* fdn_matrix_;
    int old_fdn_matrix_ = -1;
    IntChoiceParameter* reverb_type_;
    FDNData fdn_reverb_data_;

    std::string_view GetFxType() const override { return "reverb"; }
};
}