#pragma once

#include <array>
#include "engine/forward_decalre.h"
#include "ui/WrapDropBox.h"
#include "ui/wrap_check_box.h"
#include "ui/Knob.h"

namespace mana {
class AboutLayout {
public:
    AboutLayout(Synth& synth);

    void Paint();
    void SetBounds(int x, int y, int w, int h);
private:
};
}
