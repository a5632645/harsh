#include "timber_gen.h"

#include <cassert>

namespace mana {
void TimberGen::Init(float sample_rate) {
    dual_saw_.Init(sample_rate);
    sync_.Init(sample_rate);
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

void TimberGen::OnUpdateTick(const SynthParam& params, int skip, int module_idx) {
    timber_type_ = param::TimberType::GetEnum(params.timber.osc_args[module_idx].timber_type);
    dual_saw_.OnUpdateTick(params, skip, module_idx);
    sync_.OnUpdateTick(params, skip, module_idx);
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