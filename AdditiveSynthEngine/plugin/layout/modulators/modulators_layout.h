#pragma once

#include "engine/forward_decalre.h"
#include <vector>
#include <juce_gui_basics/juce_gui_basics.h>
#include "layout/modu_container.h"

namespace mana {
class ModulatorsLayout : public juce::Component, public ModuContainer {
public:
    ModulatorsLayout(Synth& synth);

    void resized() override;
private:
    juce::TabbedComponent tabbed_;
    std::vector<std::unique_ptr<juce::Component>> components_;
};
}