#include "final_fx_layout.h"

#include <ranges>
#include <algorithm>

namespace mana {
FinalFxLayout::FinalFxLayout(Synth& synth) {
    for (int i = 0; i < 5; ++i) {
        sp_effects_.emplace_back(std::make_unique<EffectLayout>(synth, i));
    }
}

void FinalFxLayout::Paint() {
    for (auto& p : sp_effects_) {
        p->Paint();
    }
}

void FinalFxLayout::SetBounds(int x, int y, int w, int h) {
    for (int i = 0; auto & e : sp_effects_) {
        e->SetBounds(x + i * 150, y, 150, h);
        ++i;
    }
}
}