#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "engine/forward_decalre.h"
#include "ui/wrap_slider.h"

namespace mana {
class VolEnvLayout : public juce::Component {
public:
    VolEnvLayout(Synth& synth);
    void resized() override;
private:
    std::unique_ptr<WrapSlider> predelay_;
    std::unique_ptr<WrapSlider> attack_;
    std::unique_ptr<WrapSlider> hold_;
    std::unique_ptr<WrapSlider> decay_;
    std::unique_ptr<WrapSlider> sustain_;
    std::unique_ptr<WrapSlider> release_;
    std::unique_ptr<WrapSlider> peak_;
    std::unique_ptr<WrapSlider> high_scale_;
};
}