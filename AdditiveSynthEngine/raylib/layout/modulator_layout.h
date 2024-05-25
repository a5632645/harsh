#pragma once

#include <array>
#include <raylib.h>
#include <memory>
#include "engine/forward_decalre.h"
#include "lfo_layout.h"
#include "envelop_layout.h"

namespace mana {
class ModulatorLayout {
public:
    ModulatorLayout(Synth& synth);
    void Paint();
    void SetBounds(Rectangle bound);
private:
    std::array<std::unique_ptr<LfoLayout>, 8> lfo_layouts_;
    std::array<std::unique_ptr<EnvelopLayout>, 8> envelop_layouts_;
};
}