#pragma once

#include <vector>
#include <raylib.h>
#include "engine/forward_decalre.h"

namespace mana {
class Spectrum {
public:
    Spectrum(Synth& synth) : synth_(synth) {}

    void Paint();
    void SetBounds(Rectangle bound);
private:
    Synth& synth_;
    Rectangle bound_;
};
}