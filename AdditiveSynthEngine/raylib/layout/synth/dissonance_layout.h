#pragma once

#include <array>
#include "engine/forward_decalre.h"
#include "ui/Knob.h"
#include "ui/WrapDropBox.h"
#include "ui/wrap_check_box.h"

namespace mana {
class DissonanceLayout {
public:
    DissonanceLayout(Synth& synth);

    void Paint();
    void SetBounds(int x, int y, int w, int h);
private:
    void OnDissonanceTypeChanged(int c);
    WrapCheckBox is_enable_;
    WrapDropBox type_;
    std::array<Knob, 2> arg_knobs_;
};
}
