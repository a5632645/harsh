#include "final_filter_layout.h"

namespace mana {
FinalFilterLayout::FinalFilterLayout(Synth & synth)
    : route_(synth)
    , filter1_(synth, 0)
    , filter2_(synth, 1) {
    addAndMakeVisible(filter1_);
    addAndMakeVisible(filter2_);
    addAndMakeVisible(route_);
}

void FinalFilterLayout::resized() {
    filter1_.setBounds(0, 0, 200, getHeight());
    filter2_.setBounds(0 + 200, 0, 200, getHeight());
    route_.setBounds(0 + 400, 0, getWidth() - 400, getHeight());
}
}