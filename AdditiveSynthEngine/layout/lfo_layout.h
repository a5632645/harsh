#pragma once

#include "engine/synth.h"
#include "ui/Knob.h"
#include "ui/WrapDropBox.h"
#include "ui/wrap_check_box.h"

namespace mana {
class LfoLayout {
public:
    LfoLayout(Synth& synth, int idx);
    void Paint();
    void SetBounds(Rectangle bound);
private:
    Knob rate_;
    Knob start_phase_;
    Knob level_;
    WrapCheckBox restart_;
    WrapDropBox wave_type_;
    WrapDropBox wave_curve_idx_;
};
}