#pragma once

#include <array>
#include "engine/forward_decalre.h"
#include "ui/WrapDropBox.h"
#include "ui/wrap_check_box.h"
#include "ui/Knob.h"

namespace mana {
class EffectLayout {
public:
    EffectLayout(Synth& synth, int effect_idx);

    void Paint();
    void SetBounds(int x, int y, int w, int h);
private:
    void OnEffectTypeChanged(int c);

    const int effect_idx;
    WrapCheckBox is_enable_;
    WrapDropBox effect_type_;
    std::array<Knob, 6> arg_knobs_;
};
}
