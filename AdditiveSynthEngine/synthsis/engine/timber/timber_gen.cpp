#include "timber_gen.h"

#include <cassert>
#include "engine/oscillor_param.h"

namespace mana {
TimberGen::TimberGen(int idx)
    : idx_(idx) {
}

void TimberGen::Init(float sample_rate, float update_rate) {
    std::apply([sample_rate, update_rate](auto&... t) { int _[]{ (t.Init(sample_rate, update_rate), 0)... }; }, timber_gens_);
}

void TimberGen::Process(TimberFrame& frame) {
    using enum param::TimberType::ParamEnum;

    auto timber_type_ = param::TimberType::GetEnum(timber_type_arg_->GetInt());
    switch (timber_type_) {
    case kDualSaw:
        std::get<DualSaw>(timber_gens_).Process(frame);
        break;
    case kSync:
        std::get<Sync>(timber_gens_).Process(frame);
        break;
    case kNoise:
        std::get<Noise>(timber_gens_).Process(frame);
        break;
    case kPwm:
        std::get<PWM>(timber_gens_).Process(frame);
        break;
    case kImpulse:
        std::get<Impulse>(timber_gens_).Process(frame);
        break;
    case kAdsrEnv:
        std::get<MultiEnvelop>(timber_gens_).Process(frame);
        break;
    default:
        assert("unkown timber type");
        break;
    }
}

void TimberGen::PrepareParams(OscillorParams& params) {
    timber_type_arg_ = params.GetParam<IntChoiceParameter>("timber.osc{}.type", idx_);
    for (int i = 0; auto & parg : osc_param_.args) {
        parg = params.GetPolyFloatParam("timber.osc{}.arg{}", idx_, i++);
    }
    std::get<MultiEnvelop>(timber_gens_).PrepareParam(params);
}

void TimberGen::OnUpdateTick() {
    std::apply([this](auto&... t) { int _[]{ (t.OnUpdateTick(osc_param_),0)... }; }, timber_gens_);
}

void TimberGen::OnNoteOn(int note) {
    std::apply([note](auto&... t) { int _[]{ (t.OnNoteOn(note), 0)... }; }, timber_gens_);
}

void TimberGen::OnNoteOff() {
    std::apply([](auto&... t) { int _[]{ (t.OnNoteOff(),0)... }; }, timber_gens_);
}
}