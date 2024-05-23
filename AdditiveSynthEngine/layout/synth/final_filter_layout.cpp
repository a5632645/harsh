#include "final_filter_layout.h"

namespace mana {
FinalFilterLayout::FinalFilterLayout(Synth & synth)
    : route_(synth)
    , filter1_(synth, 0)
    , filter2_(synth, 1) {
}

void FinalFilterLayout::Paint() {
    route_.Paint();
    filter1_.Paint();
    filter2_.Paint();
}

void FinalFilterLayout::SetBounds(Rectangle bound) {
    filter1_.SetBounds(bound.x, bound.y, 200, bound.height);
    filter2_.SetBounds(bound.x + 200, bound.y, 200, bound.height);
    route_.SetBounds({ bound.x + 400, bound.y, bound.width - 400, bound.height });
}
}