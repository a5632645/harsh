#include "envelop_layout.h"

#include "engine/synth.h"
#include "param/envelop_param.h"
#include "layout/gui_param_pack.h"

namespace mana {
EnvelopLayout::EnvelopLayout(Synth& synth, int idx) {
    const auto& param_bank = synth.GetParamBank();

    predelay_ = std::make_unique<WrapSlider>(param_bank.GetParamPtr("envelop{}.predelay", idx));
    attack_ = std::make_unique<WrapSlider>(param_bank.GetParamPtr("envelop{}.attack", idx));
    hold_ = std::make_unique<WrapSlider>(param_bank.GetParamPtr("envelop{}.hold", idx));
    decay_ = std::make_unique<WrapSlider>(param_bank.GetParamPtr("envelop{}.decay", idx));
    sustain_ = std::make_unique<WrapSlider>(param_bank.GetParamPtr("envelop{}.sustain", idx));
    release_ = std::make_unique<WrapSlider>(param_bank.GetParamPtr("envelop{}.release", idx));
    peak_ = std::make_unique<WrapSlider>(param_bank.GetParamPtr("envelop{}.peak", idx));
    att_exp_ = std::make_unique<WrapSlider>(param_bank.GetParamPtr("envelop{}.att_exp", idx));
    dec_exp_ = std::make_unique<WrapSlider>(param_bank.GetParamPtr("envelop{}.dec_exp", idx));
    rel_exp_ = std::make_unique<WrapSlider>(param_bank.GetParamPtr("envelop{}.rel_exp", idx));

    addAndMakeVisible(predelay_.get());
    addAndMakeVisible(attack_.get());
    addAndMakeVisible(hold_.get());
    addAndMakeVisible(peak_.get());
    addAndMakeVisible(decay_.get());
    addAndMakeVisible(sustain_.get());
    addAndMakeVisible(release_.get());
    addAndMakeVisible(att_exp_.get());
    addAndMakeVisible(dec_exp_.get());
    addAndMakeVisible(rel_exp_.get());
}

void EnvelopLayout::resized() {
    auto b = juce::Rectangle{ 0,50,50,50 };
    predelay_->setBounds(b);
    b.translate(50, 0);
    attack_->setBounds(b);
    att_exp_->setBounds(b.translated(0, 50).withSize(45,45));
    b.translate(50, 0);
    hold_->setBounds(b);
    peak_->setBounds(b.translated(25, -50).withSize(45, 45));
    b.translate(50, 0);
    decay_->setBounds(b);
    dec_exp_->setBounds(b.translated(0, 50).withSize(45, 45));
    sustain_->setBounds(b.translated(25, -50).withSize(45, 45));
    b.translate(50, 0);
    release_->setBounds(b);
    rel_exp_->setBounds(b.translated(0, 50).withSize(45, 45));
}
}