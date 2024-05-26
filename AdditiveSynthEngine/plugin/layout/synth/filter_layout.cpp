#include "filter_layout.h"

#include <algorithm>
#include "param/filter_param.h"
#include "layout/gui_param_pack.h"
#include "engine/synth.h"

namespace mana {
mana::FilterLayout::FilterLayout(Synth& synth, int idx) {
    filter_type_ = std::make_unique<WrapDropBox>(synth.GetParamBank().GetParamPtr<IntChoiceParameter>("filter{}.type", idx));
    filter_type_->addListener(this);

    for (int i = 0; auto & knob : arg_filter_knobs_) {
        knob = std::make_unique<WrapSlider>(synth.GetParamBank().GetParamPtr(std::format("filter{}.arg{}", idx, i++)));
    }
    arg_filter_knobs_[param::Filter_ResonanceType::kArgIdx]->addListener(this);

    addAndMakeVisible(filter_type_.get());
    for (const auto& k : arg_filter_knobs_) {
        addAndMakeVisible(*k);
    }

    // init
    OnFilterTypeChanged(0);
    OnResonanceTypeChanged(0);
}

void FilterLayout::resized() {
    filter_type_->setBounds(0, 0, getWidth(), 16);
    for (int i = 0; i < 4; ++i) {
        arg_filter_knobs_[i]->setBounds(50 * i, 16, 50, 50);
    }
    for (int i = 4; i < 8; ++i) {
        arg_filter_knobs_[i]->setBounds(50 * (i - 4), 86, 50, 50);
    }
}

void FilterLayout::OnResonanceTypeChanged(int c) {
    using enum param::Filter_ResonanceType::ParamEnum;

    auto type = param::Filter_ResonanceType::GetEnum(c);
    switch (type) {
    case kRamp:
    case kCos:
    case kParabola:
        SetSingeKnobInfo(arg_filter_knobs_[param::Filter_Resonance::kArgIdx], param::Filter_Resonance{});
        SetSingeKnobInfo(arg_filter_knobs_[param::Filter_ResonanceWidth::kArgIdx], param::Filter_ResonanceWidth{});
        arg_filter_knobs_[param::Filter_PhaserResoCycles::kArgIdx]->setVisible(false);
        //SetGuiKnobs(arg_reso_knobs_,
        //            param::Filter_Resonance{},
        //            param::Filter_ResonanceWidth{});
        break;
    case kPhaser:
        SetSingeKnobInfo(arg_filter_knobs_[param::Filter_Resonance::kArgIdx], param::Filter_Resonance{});
        SetSingeKnobInfo(arg_filter_knobs_[param::Reso_PhaserResoWidth::kArgIdx], param::Reso_PhaserResoWidth{});
        SetSingeKnobInfo(arg_filter_knobs_[param::Filter_PhaserResoCycles::kArgIdx], param::Filter_PhaserResoCycles{});
        //SetGuiKnobs(arg_reso_knobs_,
        //            param::Filter_Resonance{},
        //            param::Reso_PhaserResoWidth{},
        //            param::Filter_PhaserResoCycles{});
        break;
    default:
        assert(false);
        break;
    }
}

void FilterLayout::OnFilterTypeChanged(int c) {
    using enum param::Filter_Type::ParamEnum;

    auto type = param::Filter_Type::GetEnum(c);
    switch (type) {
    case kLowpass:
    case kHighpass:
        SetGuiKnobs(arg_filter_knobs_,
                    param::Filter_Cutoff{},
                    param::Filter_Slope{},
                    param::Filter_Knee{},
                    param::Filter_ResonanceType{});
        OnResonanceTypeChanged(last_resonance_type_);
        break;
    case kBandpass:
    case kBandstop:
        SetGuiKnobs(arg_filter_knobs_,
                    param::Filter_Cutoff{},
                    param::Filter_Slope{},
                    param::Filter_BandWidth{},
                    param::Filter_Knee{},
                    param::Filter_ResonanceType{});
        OnResonanceTypeChanged(last_resonance_type_);
        break;
    case kCombFilter:
        SetGuiKnobs(arg_filter_knobs_,
                    param::Filter_Cutoff{},
                    param::Filter_CombPhase{},
                    param::Filter_CombDepth{},
                    param::Filter_CombPhaser{},
                    param::Filter_CombShape{},
                    param::Filter_KeyTracking{});
        break;
    case kPhaser:
        SetGuiKnobs(arg_filter_knobs_,
                    param::Filter_Cutoff{},
                    param::Filter_PhaserShape{},
                    param::Filter_PhaserPhase{},
                    param::Filter_PhaserNotches{},
                    param::Filter_PhaserDepth{},
                    param::Filter_PhaserWidth{});
        break;
    case kFormant:
        SetGuiKnobs(arg_filter_knobs_,
                    param::VowelFilter_Formant{},
                    param::VowelFilter_Resonance{},
                    param::VowelFilter_Select{},
                    param::VowelFilter_Singer{},
                    param::VowelFilter_Slope{});
        break;
    }
}

void FilterLayout::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) {
    OnFilterTypeChanged(comboBoxThatHasChanged->getSelectedItemIndex());
}

void FilterLayout::sliderValueChanged(juce::Slider* slider) {
    auto reso_type = param::Filter_ResonanceType::GetChoiceIndex(slider->getValue());
    if (reso_type != last_resonance_type_) {
        last_resonance_type_ = reso_type;
        OnResonanceTypeChanged(reso_type);
    }
}
}