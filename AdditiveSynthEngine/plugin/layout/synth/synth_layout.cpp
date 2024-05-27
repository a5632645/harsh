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

void SynthLayout::BeginHighlightModulator(std::string_view id) {
    timber_.BeginHighlightModulator(id);
    unison_.BeginHighlightModulator(id);
    diss_.BeginHighlightModulator(id);
    phase_.BeginHighlightModulator(id);
    filter_.BeginHighlightModulator(id);
}

void SynthLayout::StopHighliteModulator() {
    timber_.StopHighliteModulator();
    unison_.StopHighliteModulator();
    diss_.StopHighliteModulator();
    phase_.StopHighliteModulator();
    filter_.StopHighliteModulator();
}
}