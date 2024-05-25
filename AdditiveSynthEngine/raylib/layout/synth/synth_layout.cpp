#include "synth_layout.h"

#include "engine/synth.h"

namespace mana {
SynthLayout::SynthLayout(Synth& synth)
    : timber_(synth)
    , unison_(synth.GetSynthParams())
    , diss_(synth)
    , phase_(synth)
    , filter_(synth) {
}

void SynthLayout::Paint() {
    timber_.Paint();
    unison_.Paint();
    diss_.Paint();
    phase_.Paint();
    filter_.Paint();
}

void SynthLayout::SetBounds(Rectangle bound) {
    timber_.SetBounds(bound.x, bound.y, 250, 200);
    unison_.SetBounds({ bound.x + 250, bound.y, 100, 200 });
    diss_.SetBounds(bound.x + 350, bound.y, 100, 200);
    phase_.SetBounds({ bound.x + 450, bound.y, 50, 200 });
    filter_.SetBounds({ bound.x, bound.y + 200, bound.width, bound.height - 200 });
}
}