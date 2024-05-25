#pragma once

#include "ui/Knob.h"
#include "ui/WrapDropBox.h"
#include "engine/forward_decalre.h"

namespace mana {
class UnisonLayout {
public:
    UnisonLayout(SynthParams& params);
    void Paint();
    void SetBounds(Rectangle bound);
private:
    WrapDropBox type_;
    WrapDropBox num_voice_;
    WrapSlider pitch_;
    WrapSlider phase_;
    WrapSlider pan_;
};
}