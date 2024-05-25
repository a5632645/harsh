#include "SynthLayout.h"

#include <AudioFile.h>
#include "param/standard_param.h"
#include "param/timber_param.h"
#include "param/param.h"
#include "engine/synth.h"

namespace mana {
SynthLayout::SynthLayout(Synth& synth)
    : synth_(synth)
    , scope_(synth)
    , dissonance_layout_(synth)
    , timber_layout_(synth)
    , filter_layout_(synth)
    , effect_layout0_(synth, 0)
    , effect_layout1_(synth, 1)
    , effect_layout2_(synth, 2)
    , effect_layout3_(synth, 3)
    , effect_layout4_(synth, 4)
    , resynthsis_layout_(synth)
    , standard_layout_(synth)
    , unison_layout_(synth.GetSynthParams()) {
}

void SynthLayout::paint() {
    scope_.Paint();
    timber_layout_.Paint();
    standard_layout_.Paint();
    dissonance_layout_.Paint();
    filter_layout_.Paint();
    effect_layout0_.Paint();
    effect_layout1_.Paint();
    effect_layout2_.Paint();
    effect_layout3_.Paint();
    effect_layout4_.Paint();
    resynthsis_layout_.Paint();
    unison_layout_.Paint();
}

void SynthLayout::SetBounds(int x, int y, int w, int h) {
    resynthsis_layout_.SetBounds(x + 0, y + 156 * 2 + 30 + 16, w, h - (156 * 2 + 30 + 16));
    scope_.SetBounds(x, y, w, h);
    timber_layout_.SetBounds(x + 0, y + 0, 200, 12 + 70);
    dissonance_layout_.SetBounds(x + 250, y + 0, 100, 152);
    filter_layout_.SetBounds(x + 350, y + 0, 150, 156);
    effect_layout0_.SetBounds(x + 0, y + 156 + 16, 150, 156);
    effect_layout1_.SetBounds(x + 150, y + 156 + 16, 150, 156);
    effect_layout2_.SetBounds(x + 300, y + 156 + 16, 150, 156);
    effect_layout3_.SetBounds(x + 450, y + 156 + 16, 150, 156);
    effect_layout4_.SetBounds(x + 600, y + 156 + 16, 150, 156);
    standard_layout_.SetBounds(x + w - 100, y, 100, 70);
    unison_layout_.SetBounds({ x + w - 150.0f, y + 170.0f, 150.0f, 156.0f });
}
}