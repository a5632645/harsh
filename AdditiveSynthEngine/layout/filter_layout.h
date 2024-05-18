#pragma once

#include <array>
#include "ui/WrapDropBox.h"
#include "ui/Knob.h"
#include "engine/forward_decalre.h"

namespace mana {
class FilterLayout {
public:
    FilterLayout(Synth& synth);

    void Paint();
    void SetBounds(int x, int y, int w, int h);
    void OnResonanceTypeChanged(int c);
private:
    void OnFilterTypeChanged(int c);
    WrapDropBox filter_type_;
    WrapDropBox reso_type_;
    std::array<Knob, 6> arg_filter_knobs_;
    std::array<Knob, 6> arg_reso_knobs_;
};
}
