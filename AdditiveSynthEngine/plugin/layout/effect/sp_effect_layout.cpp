#include "sp_effect_layout.h"

#include "param/param.h"
#include "param/effect_param.h"
#include "engine/synth.h"
#include "layout/gui_param_pack.h"

namespace mana {
EffectLayout::EffectLayout(Synth& synth, int effect_idx)
    : effect_idx_(effect_idx) {
    is_enable_ = std::make_unique<WrapCheckBox>(synth.GetParamBank().GetParamPtr<BoolParameter>(std::format("effect{}.enable", effect_idx)));
    effect_type_ = std::make_unique<WrapDropBox>(synth.GetParamBank().GetParamPtr<IntChoiceParameter>(std::format("effect{}.type", effect_idx)));
    effect_type_->addListener(this);

    for (int i = 0; i < 6; ++i) {
        arg_knobs_.emplace_back(std::make_unique<WrapSlider>(synth.GetParamBank().GetParamPtr(std::format("effect{}.arg{}", effect_idx, i))));
    }

    // add
    for (const auto& k : arg_knobs_) {
        addAndMakeVisible(k.get());
    }
    addAndMakeVisible(is_enable_.get());
    addAndMakeVisible(effect_type_.get());

    // init
    comboBoxChanged(effect_type_.get());
}

void EffectLayout::resized() {
    auto b = getLocalBounds();
    auto top = b.removeFromTop(20);
    is_enable_->setBounds(top.removeFromLeft(30));
    effect_type_->setBounds(top);

    auto line1 = b.removeFromTop(50);
    for (int i = 0; i < 3; ++i) {
        arg_knobs_[i]->setBounds(line1.removeFromLeft(50));
    }

    auto line2 = b.removeFromTop(50);
    for (int i = 3; i < 6; ++i) {
        arg_knobs_[i]->setBounds(line2.removeFromLeft(50));
    }
}

void EffectLayout::paint(juce::Graphics& g) {
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds(), 1);
}

void EffectLayout::OnEffectTypeChanged(int c) {
    auto type = param::EffectType::GetEnum(c);
    using enum param::EffectType::ParamEnum;
    switch (type) {
    case kOctaver:
        SetGuiKnobs(arg_knobs_,
                    param::Octaver_Amount{},
                    param::Octaver_Decay{},
                    param::Octaver_Width{});
        break;
    case kReverb:
        SetGuiKnobs(arg_knobs_,
                    param::Reverb_Amount{},
                    param::Reverb_Attack{},
                    param::Reverb_Damp{},
                    param::Reverb_Decay{},
                    param::Reverb_Speed{});
        break;
    case kPhaser:
        SetGuiKnobs(arg_knobs_,
                    param::Phaser_BarberRate{},
                    param::Phaser_Cycles{},
                    param::Phaser_Mix{},
                    param::Phaser_Pinch{},
                    param::Phaser_Mode{},
                    param::Phaser_Shape{});
        break;
    case kScramble:
        SetGuiKnobs(arg_knobs_,
                    param::Scramble_Range{},
                    param::Scramble_Rate{});
        break;
    case kDecay:
        SetGuiKnobs(arg_knobs_,
                    param::Decay_Slope{},
                    param::Decay_Time{});
        break;
    case kHarmonicDelay:
        SetGuiKnobs(arg_knobs_,
                    param::Delay_Feedback{},
                    param::Delay_Time{},
                    param::Delay_CustomFeedback{},
                    param::Delay_CustomTime{},
                    param::Delay_FeedbackSmear{});
        break;
    case kSpectralGate:
        SetGuiKnobs(arg_knobs_,
                    param::SpectralGate_Level{});
        break;
    default:
        SetGuiKnobs(arg_knobs_);
        break;
    }
}

void EffectLayout::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) {
    OnEffectTypeChanged(comboBoxThatHasChanged->getSelectedItemIndex());
}
}