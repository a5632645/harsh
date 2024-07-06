#include "reverb_section.h"

#include <param/timefx/reverb_param.h>
#include <engine/synth.h>

namespace mana {
ReverbSection::ReverbSection(Synth& s) {
    setComponentID("reverb");
    auto& pb = s.GetSynthParams().GetParamBank();

    title_ = std::make_unique<juce::Label>("", "Reverb");
    addAndMakeVisible(*title_);
    is_enable_ = std::make_unique<WrapCheckBox>(pb.GetParamPtr<BoolParameter>("timefx.reverb.enable"));
    damp_ = std::make_unique<WrapSlider>(pb.GetParamPtr(param::TimeReverb_Damp::kId));
    dry_ = std::make_unique<WrapSlider>(pb.GetParamPtr(param::TimeReverb_Dry::kId));
    wet_ = std::make_unique<WrapSlider>(pb.GetParamPtr(param::TimeReverb_Wet::kId));
    roomsize_ = std::make_unique<WrapSlider>(pb.GetParamPtr(param::TimeReverb_RoomSize::kId));
    width_ = std::make_unique<WrapSlider>(pb.GetParamPtr(param::TimeReverb_Width::kId));
    addAndMakeVisible(*is_enable_);
    addAndMakeVisible(*damp_);
    addAndMakeVisible(*dry_);
    addAndMakeVisible(*wet_);
    addAndMakeVisible(*roomsize_);
    addAndMakeVisible(*width_);

    distribution_ = std::make_unique<WrapSlider>(pb.GetParamPtr(param::TimeReverb_Distribution::kId));
    separate_ = std::make_unique<WrapSlider>(pb.GetParamPtr(param::TimeReverb_Separate::kId));
    feedback2_ = std::make_unique<WrapSlider>(pb.GetParamPtr(param::TimeReverb_Feedback2::kId));
    feedback3_ = std::make_unique<WrapSlider>(pb.GetParamPtr(param::TimeReverb_Feedback3::kId));
    freq_shift_ = std::make_unique<WrapSlider>(pb.GetParamPtr(param::TimeReverb_FreqShift::kId));
    fdn_matrix_ = std::make_unique<WrapDropBox>(pb.GetParamPtr<IntChoiceParameter>(param::TimeReverb_FdnMatrix::kId));
    reverb_type_ = std::make_unique<WrapDropBox>(pb.GetParamPtr<IntChoiceParameter>(param::TimeReverb_ReverbType::kId));
    reverb_type_->addListener(this);
    addAndMakeVisible(*distribution_);
    addAndMakeVisible(*separate_);
    addAndMakeVisible(*feedback2_);
    addAndMakeVisible(*feedback3_);
    addAndMakeVisible(*freq_shift_);
    addAndMakeVisible(*fdn_matrix_);
    addAndMakeVisible(*reverb_type_);

    comboBoxChanged(reverb_type_.get());
}

void ReverbSection::resized() {
    auto b = getLocalBounds();
    auto top = b.removeFromTop(20);
    is_enable_->setBounds(top.removeFromLeft(30));
    title_->setBounds(top.removeFromLeft(50));
    reverb_type_->setBounds(top.withHeight(25));

    auto top2 = b.removeFromTop(50);
    auto top22 = top2;
    damp_->setBounds(top2.removeFromLeft(50));
    roomsize_->setBounds(top2.removeFromLeft(50));
    width_->setBounds(top2.removeFromLeft(50));
    // fdn
    distribution_->setBounds(top22.removeFromLeft(50));
    top22.removeFromLeft(50); // share room size
    separate_->setBounds(top22.removeFromLeft(50));
    feedback2_->setBounds(top22.removeFromLeft(50));

    auto down = b.removeFromTop(50);
    dry_->setBounds(down.removeFromLeft(50));
    wet_->setBounds(down.removeFromLeft(50));
    // fdn
    freq_shift_->setBounds(down.removeFromLeft(50));
    feedback3_->setBounds(down.removeFromLeft(50));
    fdn_matrix_->setBounds(b.removeFromBottom(25));
}

void ReverbSection::paint(juce::Graphics& g) {
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds());
}

void ReverbSection::SetShowFdnUi(bool show) {
    distribution_->setVisible(show);
    separate_->setVisible(show);
    feedback2_->setVisible(show);
    feedback3_->setVisible(show);
    freq_shift_->setVisible(show);
    fdn_matrix_->setVisible(show);
}

void ReverbSection::SetShowFreeverbUi(bool show) {
    damp_->setVisible(show);
    width_->setVisible(show);
}

void ReverbSection::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) {
    auto type = param::TimeReverb_ReverbType::GetEnum(reverb_type_->getSelectedItemIndex());
    using trrt = param::TimeReverb_ReverbType::ParamEnum;
    switch (type) {
    case trrt::kFdnReverb:
        SetShowFdnUi(true);
        SetShowFreeverbUi(false);
        break;
    case trrt::kFreeverb:
        SetShowFdnUi(false);
        SetShowFreeverbUi(true);
        break;
    default:
        jassertfalse;
        break;
    }
}
}