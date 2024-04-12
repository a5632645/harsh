#include "UIMaker.h"

#include <cassert>
#include "ChooseKnob.h"
#include "engine/modulation/ChoiceParameter.h"

namespace mana {
static std::shared_ptr<Knob> MakeFloatKnob(Parameter* param) {
    auto x = std::make_shared<Knob>();
    x->set_title(param->get_name())
        .set_range(param->get_min(), param->get_max(), param->get_step(), param->get_default_value())
        .set_value(param->get_default_value())
        .set_parameter(param);
    return x;
}

static std::shared_ptr<Knob> MakeChoiceKnob(ChoiceParameter* param) {
    auto x = std::make_shared<ChooseKnob>();
    x->set_title(param->get_name())
        .set_range(param->get_min(), param->get_max(), param->get_step(), param->get_default_value())
        .set_value(param->get_default_value())
        .set_parameter(param);
    x->add_choose(param->get_choices());
    return x;
}

std::shared_ptr<Knob> UIMaker::MakeKnob(Parameter* paramter) {
    using enum Parameter::Type;

    switch (paramter->GetParamType()) {
    case kFloat:
        return MakeFloatKnob(paramter);
    case kChoice:
        return MakeChoiceKnob(static_cast<ChoiceParameter*>(paramter));
    default:
        assert(false);
        return {};
    }
}
}