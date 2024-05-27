#pragma once

#include <vector>
#include "engine/forward_decalre.h"
#include "sp_effect_layout.h"
#include "layout/modu_container.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace mana {
class FinalFxLayout : public ModuContainer, public juce::Component {
public:
    FinalFxLayout(Synth& synth);

    void resized() override;

    // 通过 ModuContainer 继承
    void BeginHighlightModulator(std::string_view id) override;
    void StopHighliteModulator() override;
private:
    std::vector<std::unique_ptr<EffectLayout>> sp_effects_;
};
}
