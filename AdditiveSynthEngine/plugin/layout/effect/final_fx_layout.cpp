#include "final_fx_layout.h"

#include <ranges>
#include <algorithm>

namespace mana {
FinalFxLayout::FinalFxLayout(Synth& synth) {
    for (int i = 0; i < 5; ++i) {
        sp_effects_.emplace_back(std::make_unique<EffectLayout>(synth, i));
        addAndMakeVisible(sp_effects_.back().get());
    }
}

void FinalFxLayout::resized() {
    for (int i = 0; auto & e : sp_effects_) {
        e->setBounds(i * 150, 0, 150, getHeight());
        ++i;
    }
}

void FinalFxLayout::BeginHighlightModulator(std::string_view id) {
    for (const auto& p : sp_effects_) {
        p->BeginHighlightModulator(id);
    }
}

void FinalFxLayout::StopHighliteModulator() {
    for (const auto& p : sp_effects_) {
        p->StopHighliteModulator();
    }
}
}