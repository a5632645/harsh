#include "master_layout.h"

#include "gui_param_pack.h"
#include "param/standard_param.h"
#include <algorithm>
#include "engine/synth.h"

namespace mana {
MasterLayout::MasterLayout(Synth& synth)
    : synth_(synth)
    , wave_scope_(synth)
    , spectrum_(synth) {
    const auto& bank = synth.GetParamBank();
    pitch_bend_.set_parameter(bank.GetParamPtr("pitch_bend"));
    //.set_title(param::PitchBend::kName);
    output_gain_.set_parameter(bank.GetParamPtr("output_gain"));
    //.set_title(param::OutputGain::kName)
}

void MasterLayout::Paint() {
    pitch_bend_.display();
    output_gain_.display();
    wave_scope_.Paint();
    spectrum_.Paint();
}

void MasterLayout::SetBounds(int x, int y, int w, int h) {
    pitch_bend_.set_bound(x, y, 50, 50);
    output_gain_.set_bound(x, y + 50, 50, 50);
    wave_scope_.SetBounds(x + 50, y, w, h / 2);
    spectrum_.SetBounds({ x + 50.0f, y + h / 2.0f, float(w), h / 2.0f });
}
}