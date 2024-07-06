#include "delay_section.h"

#include <engine/synth.h>
#include <param/timefx/delay_param.h>
#include "layout/gui_param_pack.h"

namespace mana {
DelaySection::DelaySection(Synth& s) {
    setComponentID("delay");
    title_ = std::make_unique<juce::Label>("", "Delay");
    addAndMakeVisible(*title_);

    auto& pb = s.GetSynthParams().GetParamBank();
    is_enable_ = std::make_unique<WrapCheckBox>(pb.GetParamPtr<BoolParameter>("timefx.delay.enable"));
    addAndMakeVisible(*is_enable_);
    time_mode_ = std::make_unique<WrapDropBox>(pb.GetParamPtr<IntChoiceParameter>(param::DelayTimeMode::kId));
    time_mode_->addListener(this);
    addAndMakeVisible(*time_mode_);
    delay_time_ = std::make_unique<WrapSlider>(pb.GetParamPtr("timefx.delay.time"));
    addAndMakeVisible(*delay_time_);
    feedback_ = std::make_unique<WrapSlider>(pb.GetParamPtr(param::DelayFeedback::kId));
    addAndMakeVisible(*feedback_);
    delay_mode_ = std::make_unique<WrapDropBox>(pb.GetParamPtr<IntChoiceParameter>(param::DelayMode::kId));
    addAndMakeVisible(*delay_mode_);
    feedback_center_ = std::make_unique<WrapSlider>(pb.GetParamPtr(param::DelayFbCenter::kId));
    addAndMakeVisible(*feedback_center_);
    feedback_bw_ = std::make_unique<WrapSlider>(pb.GetParamPtr(param::DelayFbBw::kId));
    addAndMakeVisible(*feedback_bw_);

    comboBoxChanged(time_mode_.get());
}

void DelaySection::resized() {
    auto b = getLocalBounds();
    auto top_bound = b.removeFromTop(20);
    is_enable_->setBounds(top_bound.removeFromLeft(30));
    title_->setBounds(top_bound);
    auto time_bound = b.removeFromTop(50);
    time_mode_->setBounds(time_bound.removeFromLeft(100).withHeight(30));
    delay_time_->setBounds(time_bound.removeFromLeft(50));
    feedback_->setBounds(time_bound.removeFromLeft(50));
    auto filter_bound = b.withHeight(50);
    feedback_center_->setBounds(filter_bound.removeFromLeft(50).withHeight(50));
    feedback_bw_->setBounds(filter_bound.removeFromLeft(50));
}

void DelaySection::paint(juce::Graphics& g) {
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds());
}

void DelaySection::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) {
    auto time_mode = param::DelayTimeMode::GetEnum(time_mode_->getSelectedItemIndex());
    using dtm = param::DelayTimeMode::ParamEnum;
    switch (time_mode) {
    case dtm::kBeats:
        SetSingeKnobInfo(delay_time_, param::DelayTimeBeat{});
        break;
    case dtm::kSeconds:
        SetSingeKnobInfo(delay_time_, param::DelayTimeSeconds{});
        break;
    default:
        jassertfalse;
        break;
    }
}
}