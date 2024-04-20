#pragma once

#include "engine/synth.h"
#include "ui/WrapDropBox.h"
#include "ui/Knob.h"

namespace mana {
class FilterLayout {
public:
    FilterLayout(Synth& synth);

    void Paint();
    void SetBounds(int x, int y, int w, int h);
private:
    void OnFilterTypeChanged(int c);
    SynthParam& synth_param_;
    WrapDropBox filter_type_;
    Knob arg0_;
    Knob arg1_;
    Knob arg2_;
    Knob arg3_;
    Knob arg4_;
    Knob arg5_;
};
}
