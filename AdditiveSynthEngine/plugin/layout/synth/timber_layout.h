#pragma once

#include "engine/forward_decalre.h"
#include "osc_layout.h"
#include "ui/wrap_slider.h"
#include "layout/modu_container.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace mana {
class TimberLayout : public ModuContainer, public juce::Component {
public:
    TimberLayout(Synth& synth);

    void resized() override;

    // 通过 ModuContainer 继承
    void BeginHighlightModulator(std::string_view id) override;
    void StopHighliteModulator() override;
private:
    OscLayout osc1_layout_;
    OscLayout osc2_layout_;
    std::unique_ptr<WrapSlider> osc2_shift_;
    std::unique_ptr<WrapSlider> osc2_beating_;
    std::unique_ptr<WrapSlider> osc1_gain_;
    std::unique_ptr<WrapSlider> osc2_gain_;
};
}
