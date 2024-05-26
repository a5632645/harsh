#include "envelop_layout.h"

#include "engine/synth.h"
#include "param/envelop_param.h"
#include "layout/gui_param_pack.h"

namespace mana {
EnvelopLayout::EnvelopLayout(Synth& synth, int idx) {
    const auto& param_bank = synth.GetParamBank();

    attack_ = std::make_unique<WrapSlider>(param_bank.GetParamPtr("envelop{}.attack", idx));
    decay_ = std::make_unique<WrapSlider>(param_bank.GetParamPtr("envelop{}.decay", idx));
    sustain_ = std::make_unique<WrapSlider>(param_bank.GetParamPtr("envelop{}.sustain", idx));
    release_ = std::make_unique<WrapSlider>(param_bank.GetParamPtr("envelop{}.release", idx));

    addAndMakeVisible(attack_.get());
    addAndMakeVisible(decay_.get());
    addAndMakeVisible(sustain_.get());
    addAndMakeVisible(release_.get());
}

void EnvelopLayout::resized() {
    attack_->setBounds(0, 0, 50, 50);
    decay_->setBounds(0 + 50, 0, 50, 50);
    sustain_->setBounds(0 + 100, 0, 50, 50);
    release_->setBounds(0 + 150, 0, 50, 50);
}
}