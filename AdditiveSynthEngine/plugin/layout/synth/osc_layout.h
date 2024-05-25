#pragma once

#include <array>
#include "engine/forward_decalre.h"
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
    const int idx_;
    WrapDropBox timber_type_;
    std::array<WrapSlider, 4> arg_knobs_;
};
}
