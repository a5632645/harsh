#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "engine/forward_decalre.h"
#include "engine/synth_params.h"

namespace mana {
class ModulatorButton : public juce::Component {
public:
    ModulatorButton(std::string_view modulator_id, Synth& synth);

    void paint(juce::Graphics& g)override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
private:
    Synth& synth_;
    std::string modulator_id_;
};
}