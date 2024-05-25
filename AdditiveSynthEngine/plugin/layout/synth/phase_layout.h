#pragma once

#include <array>
#include "engine/forward_decalre.h"
#include "ui/WrapDropBox.h"
#include "ui/Knob.h"

namespace mana {
class PhaseLayout {
public:
    PhaseLayout(Synth& synth);
    void Paint();
    void SetBounds(Rectangle bound);
private:
    void OnPhaseTypeChanged(int c);

    WrapDropBox phase_type_;
    std::array<WrapSlider, 2> phase_arg_knobs_;
};
}