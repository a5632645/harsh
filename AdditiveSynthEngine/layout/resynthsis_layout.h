#pragma once

#include "engine/synth.h"
#include "ui/Knob.h"
#include "raygui-cpp.h"

namespace mana {
class ResynthsisLayout {
public:
    ResynthsisLayout(Synth& synth);

    void Paint();
    void SetBounds(int x, int y, int w, int h);
private:
    Synth& synth_;
    rgc::CheckBox is_enable_;
    std::array<Knob, 7> arg_knobs_;
};
}