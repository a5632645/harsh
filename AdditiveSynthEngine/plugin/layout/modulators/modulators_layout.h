#pragma once

#include "engine/forward_decalre.h"
#include <vector>
#include <juce_gui_basics/juce_gui_basics.h>

namespace mana {
class ModulatorsLayout : public juce::Component {
public:
    ModulatorsLayout(Synth& synth);

    void resized() override;
private:
    juce::TabbedComponent tabbed_;
};
}