#include "phase_layout.h"

#include "engine/synth.h"
#include "layout/gui_param_pack.h"

namespace mana {
PhaseLayout::PhaseLayout(Synth& synth) {
    phase_type_ = std::make_unique<WrapDropBox>(synth.GetParamBank().GetParamPtr<IntChoiceParameter>("phase.type"));
    phase_type_->addListener(this);

    for (int i = 0; auto & arg_knob : phase_arg_knobs_) {
        arg_knob = std::make_unique<WrapSlider>(synth.GetParamBank().GetParamPtr(std::format("phase.arg{}", i++)));
    }

    addAndMakeVisible(phase_type_.get());
    for (const auto& k : phase_arg_knobs_) {
        addAndMakeVisible(*k);
    }

    comboBoxChanged(phase_type_.get());
}

void PhaseLayout::resized() {
    phase_type_->setBounds(0, 0, getWidth(), 20);
    for (int i = 0; auto & k : phase_arg_knobs_) {
        k->setBounds(0, 16 + 50 * i, 50, 50);
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
                    param::PhaseSpRandom_Amount{},
                    param::PhaseSpRandom_Smooth{});
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

void PhaseLayout::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) {
    OnPhaseTypeChanged(comboBoxThatHasChanged->getSelectedItemIndex());
}
}