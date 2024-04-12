#pragma once

#include "engine/synth.h"

namespace mana {
class SynthLayout {
public:
    SynthLayout(Synth& synth);

    void paint();
    void resized(int x, int y, int w, int h);

private:
    std::shared_ptr<Knob> ratio_;
    std::shared_ptr<Knob> beating_rate_;
    std::shared_ptr<Knob> saw_square_;
    std::shared_ptr<Knob> hard_sync_;
    std::shared_ptr<Knob> pitch_bend_;
};
}