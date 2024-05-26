#include "lfo_layout.h"

#include <format>
#include "layout/gui_param_pack.h"
#include "param/lfo_param.h"
#include "engine/synth.h"

namespace mana {
LfoLayout::LfoLayout(Synth& synth, int idx) {
    const auto& bank = synth.GetParamBank();

    rate_ = std::make_unique<WrapSlider>(bank.GetParamPtr(std::format("lfo{}.rate", idx)));
    start_phase_ = std::make_unique<WrapSlider>(bank.GetParamPtr(std::format("lfo{}.start_phase", idx)));
    level_ = std::make_unique<WrapSlider>(bank.GetParamPtr(std::format("lfo{}.level", idx)));
    restart_ = std::make_unique<WrapCheckBox>(bank.GetParamPtr<BoolParameter>(std::format("lfo{}.restart", idx)));
    wave_type_ = std::make_unique<WrapDropBox>(bank.GetParamPtr<IntChoiceParameter>(std::format("lfo{}.wave_type", idx)));

    addAndMakeVisible(rate_.get());
    addAndMakeVisible(start_phase_.get());
    addAndMakeVisible(level_.get());
    addAndMakeVisible(restart_.get());
    addAndMakeVisible(wave_type_.get());
}

void LfoLayout::resized() {
    rate_->setBounds(0, 0, 50, 50);
    start_phase_->setBounds(0 + 50, 0, 50, 50);
    level_->setBounds(0 + 100, 0, 50, 50);
    restart_->setBounds(0 + 150, 0, 16, 16);
    wave_type_->setBounds(0 + 216, 0, 100, 16);
}
}