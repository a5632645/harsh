#pragma once

#include "timefx_base.h"
#include "engine/poly_param.h"
#include "engine/oscillor_param.h"
#include "utli/smoother.h"
#include "dsp/delayline.h"

namespace mana {
class Delay : public TimeFxBase {
public:
    void Init(float sample_rate, float update_rate);
    void PrepareParams(ModulableParams& params);
    void OnUpdateTick();
    void OnNoteOn(int note) {}
    void OnNoteOff() {}
    void Process(float* pbuffer, int num) override;
private:
    BoolParameter* enable_;
    IntChoiceParameter* time_mode_;
    ModuFloatParameter* time_;
    FloatParameter* bpm_;
    IntChoiceParameter* delay_mode_;
    ModuFloatParameter* feedback_;
    utli::Smoother smooth_fb_;
    utli::Smoother smooth_time_;

    //int write_pos_{};
    //std::vector<float> delay_buffer_;
    dsp::DelayLine<float> delay_;
    float last_sample_{};
    float sample_rate_{};

    // 通过 TimeFxBase 继承
    std::string_view GetFxType() const override { return "delay"; }
};
}