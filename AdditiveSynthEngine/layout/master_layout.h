#pragma once

#include <array>
#include "engine/forward_decalre.h"
#include "ui/Knob.h"
#include "ui/WrapDropBox.h"
#include "ui/wrap_check_box.h"
#include "ui/oscilloscope.h"
#include "ui/spectrum.h"

namespace mana {
class MasterLayout {
public:
    MasterLayout(Synth& synth);

    void Paint();
    void SetBounds(int x, int y, int w, int h);

    Oscilloscope& GetWaveScope() { return wave_scope_; }
private:
    Synth& synth_;

    Knob pitch_bend_;
    Knob output_gain_;
    Oscilloscope wave_scope_;
    Spectrum spectrum_;
};
}