#pragma once

#include "engine/synth.h"
#include "ui/Knob.h"
#include "ui/WrapDropBox.h"

namespace mana {
class DissonanceLayout {
public:
    DissonanceLayout(Synth& synth);

    void Paint();
    void SetBounds(int x, int y, int w, int h);
private:
    void OnDissonanceTypeChanged(int c);
    rgc::CheckBox is_enable_;
    SynthParam& synth_param_;
    WrapDropBox type_;
    std::array<Knob, 2> arg_knobs_;
};
}
