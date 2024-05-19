#pragma once

#include <raylib.h>
#include "engine/forward_decalre.h"
#include "ui/Knob.h"

namespace mana {
class EnvelopLayout {
public:
    EnvelopLayout(Synth& synth, int idx);
    void Paint();
    void SetBounds(Rectangle bound);
private:
    Knob attack_;
    Knob decay_;
    Knob sustain_;
    Knob release_;
};
}