#include "synth_layout.h"

#include "engine/synth.h"

namespace mana {
SynthLayout::SynthLayout(Synth& synth)
    : timber_(synth)
    , unison_(synth.GetSynthParams())
    , diss_(synth)
    , phase_(synth)
    , filter_(synth) {
    addAndMakeVisible(timber_);
    addAndMakeVisible(unison_);
    addAndMakeVisible(diss_);
    addAndMakeVisible(phase_);
    addAndMakeVisible(filter_);
}

void SynthLayout::resized() {
    timber_.setBounds(0, 0, 250, 200);
    unison_.setBounds(0 + 250, 0, 100, 200);
    diss_.setBounds(0 + 350, 0, 100, 200);
    phase_.setBounds(0 + 450, 0, 50, 200);
    filter_.setBounds(0, 0 + 200, getWidth(), getHeight() - 200);
}
}