#include "chrous_section.h"

#include <engine/synth.h>
#include <param/timefx/chrous_param.h>
#include "layout/gui_param_pack.h"

namespace mana {
ChrousSection::ChrousSection(Synth& s) {
    setComponentID("chrous");
    title_ = std::make_unique<juce::Label>("", "Chrous");
    addAndMakeVisible(*title_);

    auto& pb = s.GetSynthParams().GetParamBank();
    is_enable_ = std::make_unique<WrapCheckBox>(pb.GetParamPtr<BoolParameter>("timefx.chrous.enable"));
    addAndMakeVisible(*is_enable_);
    rate_mode_ = std::make_unique<WrapDropBox>(pb.GetParamPtr<IntChoiceParameter>(param::Chrous_RateMode::kId));
    rate_mode_->addListener(this);
    addAndMakeVisible(*rate_mode_);
    chrous_rate_ = std::make_unique<WrapSlider>(pb.GetParamPtr("timefx.chrous.rate"));
    addAndMakeVisible(*chrous_rate_);
    feedback_ = std::make_unique<WrapSlider>(pb.GetParamPtr(param::Chrous_Feedback::kId));
    addAndMakeVisible(*feedback_);
    delay1_time_ = std::make_unique<WrapSlider>(pb.GetParamPtr<FloatParameter>(param::Chrous_Delay1::kId));
    addAndMakeVisible(*delay1_time_);
    delay2_time_ = std::make_unique<WrapSlider>(pb.GetParamPtr<FloatParameter>(param::Chrous_Delay2::kId));
    addAndMakeVisible(*delay2_time_);
    depth_ = std::make_unique<WrapSlider>(pb.GetParamPtr<FloatParameter>(param::Chrous_Depth::kId));
    addAndMakeVisible(*depth_);

    comboBoxChanged(rate_mode_.get());
}

void ChrousSection::resized() {
    auto b = getLocalBounds();
    auto top_bound = b.removeFromTop(20);
    is_enable_->setBounds(top_bound.removeFromLeft(30));
    title_->setBounds(top_bound);

    auto time_bound = b.removeFromTop(50);
    rate_mode_->setBounds(time_bound.removeFromLeft(100).withHeight(30));
    chrous_rate_->setBounds(time_bound.removeFromLeft(50));
    feedback_->setBounds(time_bound.removeFromLeft(50));

    auto filter_bound = b.withHeight(50);
    delay1_time_->setBounds(filter_bound.removeFromLeft(50));
    delay2_time_->setBounds(filter_bound.removeFromLeft(50));
    depth_->setBounds(filter_bound.removeFromLeft(50));
}

void ChrousSection::paint(juce::Graphics& g) {
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds());
}

void ChrousSection::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) {
    auto time_mode = param::Chrous_RateMode::GetEnum(rate_mode_->getSelectedItemIndex());
    using rm = param::Chrous_RateMode::ParamEnum;
    switch (time_mode) {
    case rm::kBeats:
        SetSingeKnobInfo(chrous_rate_, param::Chrous_RateBeat{});
        break;
    case rm::kHz:
        SetSingeKnobInfo(chrous_rate_, param::Chrous_RateHz{});
        break;
    default:
        jassertfalse;
        break;
    }
}
}