#pragma once

#include <future>
#include "engine/synth.h"
#include "ui/Knob.h"
#include "raygui-cpp.h"
#include "engine/resynthsis/resynthsis_data.h"

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
    std::future<ResynthsisFrames> resynthsis_work_;
};
}