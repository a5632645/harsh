#pragma once

#include "engine/forward_decalre.h"
#include "dissonance_layout.h"
#include "final_filter_layout.h"
#include "timber_layout.h"
#include "unison_layout.h"
#include "phase_layout.h"
#include "layout/modu_container.h"
#include <juce_gui_basics/juce_gui_basics.h>
// one last place holder layout

namespace mana {
class SynthLayout : public ModuContainer, public juce::Component {
public:
    SynthLayout(Synth& synth);

    void resized() override;
private:
    TimberLayout timber_;
    UnisonLayout unison_;
    DissonanceLayout diss_;
    PhaseLayout phase_;
    // place holder
    FinalFilterLayout filter_;
};
}