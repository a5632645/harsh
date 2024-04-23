#include "SynthLayout.h"

#include "param/standard_param.h"
#include "param/timber.h"
#include "param/param.h"

namespace mana {
SynthLayout::SynthLayout(Synth& synth)
    : scope_(synth)
    , dissonance_layout_(synth)
    , timber_layout_(synth)
    , filter_layout_(synth)
    , effect_layout0_(synth, 0)
    , effect_layout1_(synth, 1)
    , effect_layout2_(synth, 2) {
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
    filter_layout_.Paint();
    effect_layout0_.Paint();
    effect_layout1_.Paint();
    effect_layout2_.Paint();
}

void SynthLayout::SetBounds(int x, int y, int w, int h) {
    timber_layout_.SetBounds(0, 0, 200, 12 + 70);
    pitch_bend_.set_bound(200, 0, 50, 70);
    scope_.SetBounds(x, y, w, h);
    dissonance_layout_.SetBounds(250, 0, 100, 152);
    filter_layout_.SetBounds(350, 0, 150, 156);
    effect_layout0_.SetBounds(0, 156, 150, 156);
    effect_layout1_.SetBounds(150, 156, 150, 156);
    effect_layout2_.SetBounds(300, 156, 150, 156);
}
}