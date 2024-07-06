#pragma once

#include <vector>
#include <juce_gui_basics/juce_gui_basics.h>
#include "engine/forward_decalre.h"
#include "sp_effect_layout.h"
#include "timefx_chain.h"

namespace mana {
class FinalFxLayout : public juce::Component {
public:
    FinalFxLayout(Synth& synth);

    void resized() override;
private:
    std::vector<std::unique_ptr<EffectLayout>> sp_effects_;
    std::unique_ptr<TimeFxChain> time_fx_;
};
}
