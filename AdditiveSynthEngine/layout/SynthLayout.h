#pragma once

#include "engine/synth.h"
#include "ui/oscilloscope.h"
#include "ui/Knob.h"
#include "DissonanceLayout.h"
#include "timber_layout.h"
#include "filter_layout.h"
#include "effect_layout.h"

namespace mana {
class SynthLayout {
public:
    SynthLayout(Synth& synth);

    void paint();
    void SetBounds(int x, int y, int w, int h);
    Oscilloscope& GetOsciiloscope() { return scope_; }
private:
    Knob pitch_bend_;
    Oscilloscope scope_;

    DissonanceLayout dissonance_layout_;
    TimberLayout timber_layout_;
    FilterLayout filter_layout_;
    EffectLayout effect_layout0_;
    EffectLayout effect_layout1_;
    EffectLayout effect_layout2_;
};
}
