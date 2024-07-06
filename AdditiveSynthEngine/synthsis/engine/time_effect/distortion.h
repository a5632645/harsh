#pragma once

#include "timefx_base.h"
#include "engine/poly_param.h"
#include "engine/oscillor_param.h"
#include "utli/smoother.h"
#include "dsp/statevar_filter.h"

namespace mana {
class Distortion : public TimeFxBase {
public:
    void Init(float sample_rate, float update_rate);
    void PrepareParams(ModulableParams& params);
    void OnUpdateTick();
    void OnNoteOn(int note) {}
    void OnNoteOff() {}
    void Process(float* pbuffer, int num) override;
private:
    BoolParameter* enable_;
    IntChoiceParameter* dist_type_;
    ModuFloatParameter* dist_amount_;
    IntChoiceParameter* filter_type_;
    ModuFloatParameter* filter_cutoff_;
    ModuFloatParameter* filter_mode_;
    utli::Smoother smooth_dist_amount_;
    utli::Smoother smooth_filter_cutoff_;
    utli::Smoother smooth_filter_mode_;
    float inv_sample_rate_;
    dsp::StateVarFilter<float> pre_filter_;
    dsp::StateVarFilter<float> post_filter_;

    std::string_view GetFxType() const override { return "distortion"; }
};
}