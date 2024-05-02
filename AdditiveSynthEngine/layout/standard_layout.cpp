#include "standard_layout.h"

#include "gui_param_pack.h"
#include "param/standard_param.h"

namespace mana {
StandardLayout::StandardLayout(Synth& synth)
    : synth_(synth)
    , synth_param_(synth.GetSynthParam()) {
    const auto& bank = synth.GetParamBank();
    pitch_bend_.set_parameter(bank.GetParamPtr("standard.pitch_bend"));
    output_gain_.set_parameter(bank.GetParamPtr("standard.output_gain"));

    SetSingeKnobInfo(pitch_bend_, param::PitchBend{});
    SetSingeKnobInfo(output_gain_, param::OutputGain{});
}

void StandardLayout::Paint() {
    pitch_bend_.display();
    output_gain_.display();
}

void StandardLayout::SetBounds(int x, int y, int w, int h) {
    pitch_bend_.set_bound(x, y, 50, 70);
    output_gain_.set_bound(x + 50, y, 50, 70);
}
}