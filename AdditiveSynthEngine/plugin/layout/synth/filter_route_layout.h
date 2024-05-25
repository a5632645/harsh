#pragma once

#include <raylib.h>
#include "engine/forward_decalre.h"
#include "ui/wrap_check_box.h"

namespace mana {
class FilterRouteLayout {
public:
    FilterRouteLayout(Synth& synth);
    void Paint();
    void SetBounds(Rectangle bound);
private:
    WrapCheckBox filter1_enable_;
    WrapCheckBox filter2_enable_;
    WrapCheckBox stream_type_;
};
}