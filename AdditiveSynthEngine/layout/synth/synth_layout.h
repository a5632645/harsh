#pragma once

#include "engine/forward_decalre.h"
#include "dissonance_layout.h"
#include "final_filter_layout.h"
#include "timber_layout.h"
#include "unison_layout.h"
#include "phase_layout.h"
// one last place holder layout

namespace mana {
class SynthLayout {
public:
    SynthLayout(Synth& synth);
    void Paint();
    void SetBounds(Rectangle bound);
private:
    TimberLayout timber_;
    UnisonLayout unison_;
    DissonanceLayout diss_;
    PhaseLayout phase_;
    // place holder
    FinalFilterLayout filter_;
};
}