#pragma once

#include "engine/forward_decalre.h"
#include "lfo_layout.h"
#include "envelop_layout.h"
#include <raygui-cpp/Button.h>
#include <vector>

namespace mana {
class ModulatorsLayout {
public:
    ModulatorsLayout(Synth& synth);
    void Paint();
    void SetBounds(Rectangle bound);
private:
    int current_layout_{};
    std::vector<rgc::Button> buttons_;
    std::vector<std::unique_ptr<LayoutBase>> layouts_;
};
}