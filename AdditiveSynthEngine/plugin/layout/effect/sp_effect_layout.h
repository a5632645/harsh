#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>
#include "engine/forward_decalre.h"

namespace mana {
class EffectLayout {
public:
    EffectLayout(Synth& synth, int effect_idx);

    void Paint();
    void SetBounds(int x, int y, int w, int h);
private:
    void OnEffectTypeChanged(int c);

    const int effect_idx_;
    juce::ToggleButton is_enable_;
    juce::ComboBox effect_type_;
    std::vector<std::unique_ptr<juce::Slider>> arg_knobs_;
};
}
