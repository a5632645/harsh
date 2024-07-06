#include "final_fx_layout.h"

#include <ranges>
#include <algorithm>

namespace mana {
FinalFxLayout::FinalFxLayout(Synth& synth) {
    for (int i = 0; i < 5; ++i) {
        sp_effects_.emplace_back(std::make_unique<EffectLayout>(synth, i));
        addAndMakeVisible(sp_effects_.back().get());
    }

    time_fx_ = std::make_unique<TimeFxChain>(synth);
    addAndMakeVisible(time_fx_.get());
}

void FinalFxLayout::resized() {
    auto w = getLocalBounds().toFloat().getWidth() / sp_effects_.size();
    for (int i = 0; auto & e : sp_effects_) {
        e->setBounds(juce::Rectangle{ i * w, 0.0f, w, 130.0f }.toNearestInt());
        ++i;
    }
    auto b = getLocalBounds();
    time_fx_->setBounds(b.removeFromBottom(150));
}
}