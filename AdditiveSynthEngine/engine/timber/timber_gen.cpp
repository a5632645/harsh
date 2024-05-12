#include "timber_gen.h"

#include <cassert>

namespace mana {
TimberGen::TimberGen(int idx) 
    : idx_(idx) {
}

void TimberGen::Init(float sample_rate, float update_rate) {
    dual_saw_.Init(sample_rate, update_rate);
    sync_.Init(sample_rate, update_rate);
}

void TimberGen::Process(TimberFrame& frame) {
    using enum param::TimberType::ParamEnum;
    switch (timber_type_) {
    case kDualSaw:
        dual_saw_.Process(frame);
        break;
    case kSync:
        sync_.Process(frame);
        break;
    default:
        assert("unkown timber type");
        break;
    }
}

void TimberGen::PrepareParams(OscillorParams & params) {
    timber_type_arg_ = params.GetParam<IntParameter>("timber.osc{}.type", idx_);
    for (int i = 0; auto & parg : osc_param_.args) {
        parg = params.GetPolyFloatParam("timber.osc{}.arg{}", idx_, i++);
    }
}

void TimberGen::OnUpdateTick() {
    timber_type_ = param::TimberType::GetEnum(timber_type_arg_->GetInt());
    dual_saw_.OnUpdateTick(osc_param_);
    sync_.OnUpdateTick(osc_param_);
}

void TimberGen::OnNoteOn(int note) {
    dual_saw_.OnNoteOn(note);
    sync_.OnNoteOn(note);
}

void TimberGen::OnNoteOff() {
    dual_saw_.OnNoteOff();
    sync_.OnNoteOff();
}
}