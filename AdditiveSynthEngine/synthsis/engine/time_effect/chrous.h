#pragma once

#include "timefx_base.h"
#include "engine/poly_param.h"
#include "engine/oscillor_param.h"
#include "utli/smoother.h"
#include "dsp/delayline.h"
#include "dsp/corid_osc.h"

namespace mana {
class Chrous : public TimeFxBase {
public:
    void Init(float sample_rate, float update_rate);
    void PrepareParams(ModulableParams& params);
    void OnUpdateTick();
    void OnNoteOn(int note) {}
    void OnNoteOff() {}
    void Process(float* pbuffer, int num) override;
private:
    BoolParameter* enable_;
    IntChoiceParameter* rate_mode_;
    ModuFloatParameter* rate_;
    FloatParameter* bpm_;
    ModuFloatParameter* delay1_time_;
    ModuFloatParameter* delay2_time_;
    ModuFloatParameter* depth_;
    ModuFloatParameter* feedback_;
    utli::Smoother smooth_feedback_;

    dsp::DelayLine<float> delay1_;
    utli::Smoother delay1_samples_;
    dsp::DelayLine<float> delay2_;
    utli::Smoother delay2_samples_;
    dsp::CoridOscillor<float> chorus_lfo_;
    float last_sample_{};
    float sample_rate_{};

    // 通过 TimeFxBase 继承
    std::string_view GetFxType() const override { return "chrous"; }
};
}