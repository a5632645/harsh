#include "distortion_section.h"

#include <engine/synth.h>
#include <param/timefx/distortion_param.h>
#include "layout/gui_param_pack.h"

namespace mana {
DistortionSection::DistortionSection(Synth& s) {
    setComponentID("distortion");
    title_ = std::make_unique<juce::Label>("", "Distortion");
    addAndMakeVisible(*title_);

    auto& pb = s.GetSynthParams().GetParamBank();
    is_enable_ = std::make_unique<WrapCheckBox>(pb.GetParamPtr<BoolParameter>("timefx.distortion.enable"));
    addAndMakeVisible(*is_enable_);
    distortion_type_ = std::make_unique<WrapDropBox>(pb.GetParamPtr<IntChoiceParameter>(param::DistortionType::kId));
    addAndMakeVisible(*distortion_type_);
    distortion_amount_ = std::make_unique<WrapSlider>(pb.GetParamPtr(param::DistortionAmount::kId));
    addAndMakeVisible(*distortion_amount_);
    filter_mode_ = std::make_unique<WrapSlider>(pb.GetParamPtr(param::DistortionFilterMode::kId));
    SetSingeKnobInfo(filter_mode_, param::DistortionFilterMode{});
    addAndMakeVisible(*filter_mode_);
    filter_type_ = std::make_unique<WrapDropBox>(pb.GetParamPtr<IntChoiceParameter>(param::DistortionFilterType::kId));
    addAndMakeVisible(*filter_type_);
    filter_cutoff_ = std::make_unique<WrapSlider>(pb.GetParamPtr(param::DistortionFilterCutoff::kId));
    addAndMakeVisible(*filter_cutoff_);
}

void DistortionSection::resized() {
    auto b = getLocalBounds();
    auto top_bound = b.removeFromTop(20);
    is_enable_->setBounds(top_bound.removeFromLeft(30));
    title_->setBounds(top_bound);
    auto dist_bound = b.removeFromTop(50).withHeight(50);
    auto fiter_bound = b.withHeight(50);
    distortion_type_->setBounds(dist_bound.removeFromLeft(100).withHeight(30));
    distortion_amount_->setBounds(dist_bound.withSize(50, 50));
    filter_type_->setBounds(fiter_bound.removeFromLeft(100).withHeight(30));
    filter_cutoff_->setBounds(fiter_bound.removeFromLeft(50).withHeight(50));
    filter_mode_->setBounds(fiter_bound.withSize(50, 50));
}

void DistortionSection::paint(juce::Graphics& g) {
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds());
}
}