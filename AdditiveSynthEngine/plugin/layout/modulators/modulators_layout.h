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

    // 通过 ModuContainer 继承
    void BeginHighlightModulator(std::string_view id) override;
    void StopHighliteModulator() override;
private:
    juce::TabbedComponent tabbed_;
    std::vector<std::unique_ptr<ModuContainer>> components_;
};
}