#pragma once

#include "engine/synth.h"
#include "ui/Knob.h"

namespace mana {
class StandardLayout {
public:
    StandardLayout(Synth& synth);

    void Paint();
    void SetBounds(int x, int y, int w, int h);
private:
    Synth& synth_;
    SynthParam& synth_param_;

    Knob pitch_bend_;
    Knob output_gain_;
};
}