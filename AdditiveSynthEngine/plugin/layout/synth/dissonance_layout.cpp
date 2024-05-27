#include "dissonance_layout.h"

#include <cassert>
#include "param/param.h"
#include "param/dissonance_param.h"
#include "layout/gui_param_pack.h"
#include "engine/synth.h"

namespace mana {
DissonanceLayout::DissonanceLayout(Synth& synth) {
    is_enable_ = std::make_unique<WrapCheckBox>(synth.GetParamBank().GetParamPtr<BoolParameter>("dissonance.enable"));
    type_ = std::make_unique<WrapDropBox>(synth.GetParamBank().GetParamPtr<IntChoiceParameter>("dissonance.type"));
    type_->addListener(this);

    arg_knobs_[0] = std::make_unique<WrapSlider>(synth.GetParamBank().GetParamPtr("dissonance.arg0"));
    arg_knobs_[1] = std::make_unique<WrapSlider>(synth.GetParamBank().GetParamPtr("dissonance.arg1"));

    OnDissonanceTypeChanged(0);

    addAndMakeVisible(is_enable_.get());
    addAndMakeVisible(type_.get());
    addAndMakeVisible(arg_knobs_[0].get());
    addAndMakeVisible(arg_knobs_[1].get());
}

void DissonanceLayout::resized() {
    is_enable_->setBounds(0, 0, getWidth(), 16);
    type_->setBounds(0, 12, getWidth(), 12);
    arg_knobs_[0]->setBounds(0, 24, 50, 50);
    arg_knobs_[1]->setBounds(0, 24 + 50, 50, 50);
}

void DissonanceLayout::OnDissonanceTypeChanged(int c) {
    auto type = param::DissonanceType::GetEnum(c);

    using enum param::DissonanceType::ParamEnum;
    switch (type) {
    case kString:
        SetGuiKnobs(arg_knobs_,
                    param::StringDissStretch{},
                    param::StringMultiRatio{});
        break;
    case kHarmonicStretch:
        SetGuiKnobs(arg_knobs_,
                    param::HarmonicStrech{});
        break;
    case kSemitoneSpace:
        SetGuiKnobs(arg_knobs_,
                    param::SemitoneSpace{});
        break;
    case kStaticError:
        SetGuiKnobs(arg_knobs_,
                    param::ErrorRamp{},
                    param::ErrorRange{});
        break;
    case kFakeUnison:
    case kFakeUnison2:
        SetGuiKnobs(arg_knobs_,
                    param::FakeUnisonRatio0{},
                    param::FakeUnisonRatio1{});
        break;
    case kDispersion:
        SetGuiKnobs(arg_knobs_,
                    param::Dispersion_Amount{},
                    param::Dispersion_Warp{});
        break;
    case kPitchQuantize:
        SetGuiKnobs(arg_knobs_,
                    param::PitchQuantize_Amount{});
        break;
    default:
        assert(false && "unkown type");
        break;
    }
}

void DissonanceLayout::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) {
    OnDissonanceTypeChanged(comboBoxThatHasChanged->getSelectedItemIndex());
}

void DissonanceLayout::BeginHighlightModulator(std::string_view id) {
    for(auto& knob : arg_knobs_) {
        knob->BeginHighlightModulator(id);
    }
}

void DissonanceLayout::StopHighliteModulator() {
    for (auto& knob : arg_knobs_) {
        knob->StopHighliteModulator();
    }
}
}