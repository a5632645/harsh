#pragma once

#include "engine/synth.h"
#include "ui/Knob.h"
#include "ui/WrapDropBox.h"

namespace mana {
class StandardLayout {
public:
    StandardLayout(Synth& synth);

    void Paint();
    void SetBounds(int x, int y, int w, int h);
private:
    void OnPhaseTypeChanged(int c);

    Synth& synth_;

    Knob pitch_bend_;
    Knob output_gain_;

    WrapDropBox phase_type_;
    std::array<Knob, 2> phase_arg_knobs_;
};
}