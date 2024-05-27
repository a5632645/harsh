#include "master_layout.h"

#include "gui_param_pack.h"
#include "param/standard_param.h"
#include <algorithm>
#include "engine/synth.h"

namespace mana {
MasterLayout::MasterLayout(Synth& synth)
    : synth_(synth)
    //, wave_scope_(synth)
    , spectrum_(synth) {
    const auto& bank = synth.GetParamBank();
    pitch_bend_ = std::make_unique<WrapSlider>(bank.GetParamPtr("pitch_bend"));
    output_gain_ = std::make_unique<WrapSlider>(bank.GetParamPtr("output_gain"));

    addAndMakeVisible(pitch_bend_.get());
    addAndMakeVisible(output_gain_.get());
    //addAndMakeVisible(wave_scope_.GetWidget());
    addAndMakeVisible(spectrum_);
}

void MasterLayout::resized() {
    pitch_bend_->setBounds(0, 0, 50, 50);
    output_gain_->setBounds(0, 0 + 50, 50, 50);
    //wave_scope_.SetBounds(x + 50, y, w, h / 2);
    spectrum_.setBounds(50.0f, getHeight() / 2.0f, getWidth(), getHeight() / 2.0f);
}

void MasterLayout::BeginHighlightModulator(std::string_view id) {
    pitch_bend_->BeginHighlightModulator(id);
    output_gain_->BeginHighlightModulator(id);
}

void MasterLayout::StopHighliteModulator() {
    pitch_bend_->StopHighliteModulator();
    output_gain_->StopHighliteModulator();
}
}