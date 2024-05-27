#pragma once

#include "engine/forward_decalre.h"
#include "filter_layout.h"
#include "filter_route_layout.h"
#include "layout/modu_container.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace mana {
class FinalFilterLayout : public ModuContainer, public juce::Component {
public:
    FinalFilterLayout(Synth& synth);

    void resized() override;

    // 通过 ModuContainer 继承
    void BeginHighlightModulator(std::string_view id) override;
    void StopHighliteModulator() override;
private:
    FilterRouteLayout route_;
    FilterLayout filter1_;
    FilterLayout filter2_;
};
}