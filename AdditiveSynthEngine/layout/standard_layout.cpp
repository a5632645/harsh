#include "standard_layout.h"

#include "gui_param_pack.h"
#include "param/standard_param.h"
#include <algorithm>
#include "engine/synth.h"

namespace mana {
StandardLayout::StandardLayout(Synth& synth)
    : synth_(synth)
    , phase_type_(synth.GetParamBank().GetParamPtr<IntChoiceParameter>("phase.type")) {
    const auto& bank = synth.GetParamBank();
    pitch_bend_.set_parameter(bank.GetParamPtr("pitch_bend"))
        .set_title(param::PitchBend::kName);
    output_gain_.set_parameter(bank.GetParamPtr("output_gain"))
        .set_title(param::OutputGain::kName);

    // phases
    phase_type_.SetChoices(param::PhaseType::kNames);
    phase_type_.on_choice_changed = [this](int c) {OnPhaseTypeChanged(c); };

    for (int i = 0; auto & arg_knob : phase_arg_knobs_) {
        arg_knob.set_parameter(synth.GetParamBank().GetParamPtr(std::format("phase.arg{}", i++)));
    }
    OnPhaseTypeChanged(0);
}

void StandardLayout::Paint() {
    pitch_bend_.display();
    output_gain_.display();
    std::ranges::for_each(phase_arg_knobs_, &Knob::display);
    phase_type_.Paint();
}

void StandardLayout::SetBounds(int x, int y, int w, int h) {
    pitch_bend_.set_bound(x, y, 50, 50);
    output_gain_.set_bound(x + 50, y, 50, 50);
    phase_type_.SetBounds(rgc::Bounds(x, y + 70, w, 16));
    for (int i = 0; auto & k : phase_arg_knobs_) {
        k.set_bound(x + i * 50, y + 86, 50, 50);
        ++i;
    }
}

void StandardLayout::OnPhaseTypeChanged(int c) {
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