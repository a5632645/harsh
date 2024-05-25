#include "phase_layout.h"

#include "engine/synth.h"
#include "layout/gui_param_pack.h"

namespace mana {
PhaseLayout::PhaseLayout(Synth& synth) {
    phase_type_.SetParameter(synth.GetParamBank().GetParamPtr<IntChoiceParameter>("phase.type"));
    phase_type_.on_choice_changed = [this](int c) {OnPhaseTypeChanged(c); };

    for (int i = 0; auto & arg_knob : phase_arg_knobs_) {
        arg_knob.set_parameter(synth.GetParamBank().GetParamPtr(std::format("phase.arg{}", i++)));
    }
    OnPhaseTypeChanged(0);
}

void PhaseLayout::Paint() {
    std::ranges::for_each(phase_arg_knobs_, &WrapSlider::display);
    phase_type_.Paint();
}

void PhaseLayout::SetBounds(Rectangle bound) {
    phase_type_.SetBounds({ bound.x, bound.y, bound.width, 16 });
    for (int i = 0; auto & k : phase_arg_knobs_) {
        k.set_bound(bound.x, bound.y + 16 + 50 * i, 50, 50);
        ++i;
    }
}

void PhaseLayout::OnPhaseTypeChanged(int c) {
    auto type = param::PhaseType::GetEnum(c);
    using enum param::PhaseType::ParamEnum;
    switch (type) {
    case kDispersion:
        SetGuiKnobs(phase_arg_knobs_,
                    param::PhaseDispersion_Amount{},
                    param::PhaseDispersion_Warp{});
        break;
    case  kSpectralRandom:
        SetGuiKnobs(phase_arg_knobs_,
                    param::PhaseSpRandom_Amount{});
        break;
    case kPowDistribute:
        SetGuiKnobs(phase_arg_knobs_,
                    param::PhasePowDist_Pow{});
        break;
    default:
        SetGuiKnobs(phase_arg_knobs_);
        break;
    }
}
}