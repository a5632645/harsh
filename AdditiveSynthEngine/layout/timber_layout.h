#pragma once

#include "engine/synth.h"
#include "ui/WrapDropBox.h"
#include "ui/Knob.h"

namespace mana {
class TimberLayout {
public:
    TimberLayout(Synth& synth);

    void Paint();
    void SetBounds(int x, int y, int w, int h);
private:
    void OnTimberTypeChanged(int c);
    SynthParam& synth_param_;
    WrapDropBox timber_type_;
    Knob arg0_;
    Knob arg1_;
    Knob arg2_;
    Knob arg3_;
};
}
