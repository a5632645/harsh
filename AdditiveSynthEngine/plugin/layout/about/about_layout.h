#pragma once

#include <array>
#include "engine/forward_decalre.h"

namespace mana {
class AboutLayout {
public:
    AboutLayout(Synth& synth);

    void Paint();
    void SetBounds(int x, int y, int w, int h);
private:
};
}
