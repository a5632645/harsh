#pragma once

#include "engine/synth.h"
#include "ui/WrapDropBox.h"
#include "ui/Knob.h"

namespace mana {
class OscLayout {
public:
    OscLayout(Synth& synth, int idx);

    void Paint();
    void SetBounds(int x, int y, int w, int h);
private:
    void OnTimberTypeChanged(int c);
    SynthParam& synth_param_;
    const int idx_;
    WrapDropBox timber_type_;
    std::array<Knob, 4> arg_knobs_;
};
}
