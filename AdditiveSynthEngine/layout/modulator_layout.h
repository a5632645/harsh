#pragma once

#include <array>
#include "lfo_layout.h"

namespace mana {
class ModulatorLayout {
public:
    ModulatorLayout(Synth& synth);
    void Paint();
    void SetBounds(Rectangle bound);
private:
    std::array<std::unique_ptr<LfoLayout>, 8> lfo_layouts_;
};
}