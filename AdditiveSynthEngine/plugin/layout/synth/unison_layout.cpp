#include "unison_layout.h"

#include <ranges>
#include "layout/gui_param_pack.h"
#include "engine/synth_params.h"

namespace mana {
UnisonLayout::UnisonLayout(SynthParams& params) {
    auto& bank = params.GetParamBank();
    type_ = std::make_unique<WrapDropBox>(bank.GetParamPtr<IntChoiceParameter>("unison.type"));
    num_voice_ = std::make_unique<WrapSlider>(bank.GetParamPtr<IntParameter>("unison.num_voice"));
    pitch_ = std::make_unique<WrapSlider>(bank.GetParamPtr("unison.pitch"));
    phase_ = std::make_unique<WrapSlider>(bank.GetParamPtr("unison.phase"));
    pan_ = std::make_unique<WrapSlider>(bank.GetParamPtr("unison.pan"));

    addAndMakeVisible(type_.get());
    addAndMakeVisible(num_voice_.get());
    addAndMakeVisible(pitch_.get());
    addAndMakeVisible(phase_.get());
    addAndMakeVisible(pan_.get());
}

void UnisonLayout::resized() {
    type_->setBounds(0, 0, getWidth(), 20);
    num_voice_->setBounds(0, 16, 50, 50);
    pitch_->setBounds(50, 16, 50, 50);
    phase_->setBounds(0, 16 + 50, 50, 50);
    pan_->setBounds(50, 16 + 50, 50, 50);
}

void UnisonLayout::BeginHighlightModulator(std::string_view id) {
    num_voice_->BeginHighlightModulator(id);
    pitch_->BeginHighlightModulator(id);
    phase_->BeginHighlightModulator(id);
    pan_->BeginHighlightModulator(id);
}

void UnisonLayout::StopHighliteModulator() {
    num_voice_->StopHighliteModulator();
    pitch_->StopHighliteModulator();
    phase_->StopHighliteModulator();
    pan_->StopHighliteModulator();
}
}