#pragma once

#include <vector>
#include "engine/forward_decalre.h"
#include "sp_effect_layout.h"

namespace mana {
class FinalFxLayout {
public:
    FinalFxLayout(Synth& synth);

    void Paint();
    void SetBounds(int x, int y, int w, int h);
private:
    std::vector<std::unique_ptr<EffectLayout>> sp_effects_;
};
}
