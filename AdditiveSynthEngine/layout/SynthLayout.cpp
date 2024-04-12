#include "SynthLayout.h"

#include "ui/UIMaker.h"

namespace mana {
SynthLayout::SynthLayout(Synth& synth) {
    const auto& bank = synth.GetParamBank();
    ratio_ = UIMaker::MakeKnob(bank.GetParamPtr("timber.dual_saw.ratio"));
    beating_rate_ = UIMaker::MakeKnob(bank.GetParamPtr("timber.dual_saw.beating_rate"));
    saw_square_ = UIMaker::MakeKnob(bank.GetParamPtr("timber.dual_saw.saw_square"));
    hard_sync_ = UIMaker::MakeKnob(bank.GetParamPtr("timber_proc.hard_sync.sync"));
    pitch_bend_ = UIMaker::MakeKnob(bank.GetParamPtr("voice.pitch_bend"));
}

void SynthLayout::paint() {
    ratio_->display();
    beating_rate_->display();
    saw_square_->display();
    hard_sync_->display();
    pitch_bend_->display();
}

void SynthLayout::resized(int x, int y, int w, int h) {
    ratio_->set_bound(0, 0, 50, 70);
    beating_rate_->set_bound(50, 0, 50, 70);
    saw_square_->set_bound(100, 0, 50, 70);
    hard_sync_->set_bound(150, 0, 50, 70);
    pitch_bend_->set_bound(200, 0, 50, 70);
}
}