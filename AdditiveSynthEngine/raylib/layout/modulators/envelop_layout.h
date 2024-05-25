#pragma once

#include <raylib.h>
#include "engine/forward_decalre.h"
#include "ui/Knob.h"
#include "layout_base.h"

namespace mana {
class EnvelopLayout : public LayoutBase {
public:
    EnvelopLayout(Synth& synth, int idx, std::string_view id);
    void Paint() override;
    void SetBounds(Rectangle bound) override;
private:
    Knob attack_;
    Knob decay_;
    Knob sustain_;
    Knob release_;
};
}