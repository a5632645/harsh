#include "timber_layout.h"

#include <unordered_map>
#include "param/timber_param.h"
#include "param/param.h"
#include "layout/gui_param_pack.h"
#include "engine/synth.h"

namespace mana {
TimberLayout::TimberLayout(Synth& synth)
    : osc1_layout_(synth, 0)
    , osc2_layout_(synth, 1) {
    osc2_beating_ = std::make_unique<WrapSlider>(synth.GetParamBank().GetParamPtr("timber.osc2_beating"));
    osc2_shift_ = std::make_unique<WrapSlider>(synth.GetParamBank().GetParamPtr("timber.osc2_shift"));
    osc1_gain_ = std::make_unique<WrapSlider>(synth.GetParamBank().GetParamPtr("timber.osc1_gain"));
    osc2_gain_ = std::make_unique<WrapSlider>(synth.GetParamBank().GetParamPtr("timber.osc2_gain"));

    addAndMakeVisible(osc2_beating_.get());
    addAndMakeVisible(osc2_shift_.get());
    addAndMakeVisible(osc1_gain_.get());
    addAndMakeVisible(osc2_gain_.get());
    addAndMakeVisible(&osc1_layout_);
    addAndMakeVisible(&osc2_layout_);
}

void TimberLayout::resized() {
    auto b = getLocalBounds();
    auto left = b.removeFromLeft(50);
    osc2_beating_->setBounds(left.removeFromTop(50));
    osc2_shift_->setBounds(left.removeFromTop(50));
    osc1_gain_->setBounds(left.removeFromTop(50));
    osc2_gain_->setBounds(left.removeFromTop(50));
    osc1_layout_.setBounds(b.removeFromTop(70));
    osc2_layout_.setBounds(b.removeFromTop(70));
}
}