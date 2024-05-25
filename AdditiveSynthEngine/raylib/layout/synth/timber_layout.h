#pragma once

#include "engine/forward_decalre.h"
#include "osc_layout.h"
#include "ui/Knob.h"

namespace mana {
class TimberLayout {
public:
    TimberLayout(Synth& synth);

    void Paint();
    void SetBounds(int x, int y, int w, int h);
private:
    OscLayout osc1_layout_;
    OscLayout osc2_layout_;
    Knob osc2_shift_;
    Knob osc2_beating_;
    Knob osc1_gain_;
    Knob osc2_gain_;
};
}
