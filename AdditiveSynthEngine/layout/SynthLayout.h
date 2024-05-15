#pragma once

#include "engine/forward_decalre.h"
#include "ui/oscilloscope.h"
#include "ui/Knob.h"
#include "DissonanceLayout.h"
#include "timber_layout.h"
#include "filter_layout.h"
#include "effect_layout.h"
#include "resynthsis_layout.h"
#include "standard_layout.h"
#include "unison_layout.h"

namespace mana {
class SynthLayout {
public:
    SynthLayout(Synth& synth);

    void paint();
    void SetBounds(int x, int y, int w, int h);
    Oscilloscope& GetOscilloscope() { return scope_; }
private:
    Synth& synth_;

    Oscilloscope scope_;

    StandardLayout standard_layout_;
    DissonanceLayout dissonance_layout_;
    TimberLayout timber_layout_;
    FilterLayout filter_layout_;
    EffectLayout effect_layout0_;
    EffectLayout effect_layout1_;
    EffectLayout effect_layout2_;
    EffectLayout effect_layout3_;
    EffectLayout effect_layout4_;
    ResynthsisLayout resynthsis_layout_;
    UnisonLayout unison_layout_;
};
}
