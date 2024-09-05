#include "unison_layout.h"

#include <ranges>
#include "layout/gui_param_pack.h"
#include "engine/synth_params.h"

namespace mana {
UnisonLayout::UnisonLayout(SynthParams& params) {
    auto& bank = params.GetParamBank();
    num_voice_ = std::make_unique<WrapSlider>(bank.GetParamPtr<IntParameter>("unison.num_voice"));
    pitch_ = std::make_unique<WrapSlider>(bank.GetParamPtr("unison.pitch"));
    phase_ = std::make_unique<WrapSlider>(bank.GetParamPtr("unison.phase"));
    pan_ = std::make_unique<WrapSlider>(bank.GetParamPtr("unison.pan"));
    morph_ = std::make_unique<WrapSlider>(bank.GetParamPtr("unison.morph"));
    randomness_ = std::make_unique<WrapSlider>(bank.GetParamPtr("unison.randomness"));

    addAndMakeVisible(num_voice_.get());
    addAndMakeVisible(pitch_.get());
    addAndMakeVisible(phase_.get());
    addAndMakeVisible(pan_.get());
    addAndMakeVisible(morph_.get());
    addAndMakeVisible(randomness_.get());
}

void UnisonLayout::resized() {
    num_voice_->setBounds(0, 0, 50, 50);
    pitch_->setBounds(50, 0, 50, 50);
    phase_->setBounds(0, 0 + 50, 50, 50);
    pan_->setBounds(50, 0 + 50, 50, 50);
    morph_->setBounds(0, 0 + 50 + 50, 50, 50);
    randomness_->setBounds(50, 0 + 50 + 50, 50, 50);
}
}