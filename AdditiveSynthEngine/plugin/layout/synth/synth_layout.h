#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "engine/forward_decalre.h"
#include "dissonance_layout.h"
#include "final_filter_layout.h"
#include "timber_layout.h"
#include "unison_layout.h"
#include "phase_layout.h"
#include "vol_env_layout.h"

namespace mana {
class SynthLayout : public juce::Component {
public:
    SynthLayout(Synth& synth);

    void resized() override;
private:
    TimberLayout timber_;
    UnisonLayout unison_;
    DissonanceLayout diss_;
    PhaseLayout phase_;
    VolEnvLayout vol_env_;
    FinalFilterLayout filter_;
};
}