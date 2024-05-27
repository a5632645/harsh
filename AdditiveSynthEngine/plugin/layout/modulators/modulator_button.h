#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "engine/forward_decalre.h"

namespace mana {
class ModulatorButton : public juce::Component {
public:
    ModulatorButton(std::string_view modulator_id, Synth& synth);
    ~ModulatorButton() override;

    void resized() override;
private:
    friend class WrapSlider;
    Synth& synth_;

    std::unique_ptr<juce::Label> num_modulation_label_;
    std::unique_ptr<juce::Component> drag_component_;
};
}