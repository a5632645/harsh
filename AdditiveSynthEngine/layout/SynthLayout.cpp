#include "SynthLayout.h"

#include "param/standard_param.h"
#include "param/timber.h"
#include "param/param.h"

namespace mana {
SynthLayout::SynthLayout(Synth& synth)
    : scope_(synth)
    , dissonance_layout_(synth)
    , timber_layout_(synth) {
    const auto& bank = synth.GetParamBank();

    pitch_bend_.set_parameter(bank.GetParamPtr("standard.pitch_bend"));

    pitch_bend_.set_title(param::PitchBend::kName);
    pitch_bend_.value_to_text_function = param::FloatParam<param::PitchBend>::GetText;
}

void SynthLayout::paint() {
    timber_layout_.Paint();
    pitch_bend_.display();
    scope_.Paint();
    dissonance_layout_.Paint();
}

void SynthLayout::SetBounds(int x, int y, int w, int h) {
    timber_layout_.SetBounds(0, 0, 200, 12 + 70);
    pitch_bend_.set_bound(200, 0, 50, 70);
    scope_.SetBounds(x, y, w, h);
    dissonance_layout_.SetBounds(250, 0, 100, 152);
}
}