#include "vol_env_layout.h"

#include <param/multi_env_param.h>
#include <engine/synth.h>

namespace mana {
VolEnvLayout::VolEnvLayout(Synth& synth) {
    auto& b = synth.GetSynthParams().GetParamBank();
    predelay_ = std::make_unique<WrapSlider>(b.GetParamPtr<FloatParameter>(param::VolEnv_PreDelay::kId));
    attack_ = std::make_unique<WrapSlider>(b.GetParamPtr<FloatParameter>(param::VolEnv_Attack::kId));
    hold_ = std::make_unique<WrapSlider>(b.GetParamPtr<FloatParameter>(param::VolEnv_Hold::kId));
    peak_ = std::make_unique<WrapSlider>(b.GetParamPtr<FloatParameter>(param::VolEnv_Peak::kId));
    decay_ = std::make_unique<WrapSlider>(b.GetParamPtr<FloatParameter>(param::VolEnv_Decay::kId));
    sustain_ = std::make_unique<WrapSlider>(b.GetParamPtr<FloatParameter>(param::VolEnv_Sustain::kId));
    release_ = std::make_unique<WrapSlider>(b.GetParamPtr<FloatParameter>(param::VolEnv_Release::kId));
    high_scale_ = std::make_unique<WrapSlider>(b.GetParamPtr<FloatParameter>(param::VolEnv_HighScale::kId));

    addAndMakeVisible(predelay_.get());
    addAndMakeVisible(attack_.get());
    addAndMakeVisible(hold_.get());
    addAndMakeVisible(decay_.get());
    addAndMakeVisible(sustain_.get());
    addAndMakeVisible(release_.get());
    addAndMakeVisible(high_scale_.get());
    addAndMakeVisible(peak_.get());
}

void VolEnvLayout::resized() {
    auto b = juce::Rectangle{ 0,0,50,50 };
    high_scale_->setBounds(b.translated(0, 50));
    predelay_->setBounds(b);
    b.translate(50, 0);
    attack_->setBounds(b);
    b.translate(50, 0);
    peak_->setBounds(b.translated(0, 50));
    hold_->setBounds(b);
    b.translate(50, 0);
    decay_->setBounds(b);
    sustain_->setBounds(b.translated(25, 50));
    b.translate(50, 0);
    release_->setBounds(b);
}
}