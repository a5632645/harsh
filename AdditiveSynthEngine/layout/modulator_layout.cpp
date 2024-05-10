#include "modulator_layout.h"

namespace mana {
mana::ModulatorLayout::ModulatorLayout(Synth& synth) {
    for (int i = 0; auto & p : lfo_layouts_) {
        p = std::make_unique<LfoLayout>(synth, i++);
    }
}

void mana::ModulatorLayout::Paint() {
    for (auto& p : lfo_layouts_) {
        p->Paint();
    }
}

void mana::ModulatorLayout::SetBounds(Rectangle bound) {
    for (int i = 0; auto & p : lfo_layouts_) {
        p->SetBounds({ bound.x, bound.y + 60 * i, bound.width,50 });
        ++i;
    }
}
}