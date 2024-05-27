#pragma once

#include "engine/forward_decalre.h"
#include "layout/modu_container.h"
#include "ui/wrap_slider.h"

namespace mana {
class EnvelopLayout : public juce::Component, public ModuContainer {
public:
    EnvelopLayout(Synth& synth, int idx);
    
    void resized() override;

    // 通过 ModuContainer 继承
    void BeginHighlightModulator(std::string_view id) override;
    void StopHighliteModulator() override;
private:
    std::unique_ptr<WrapSlider> attack_;
    std::unique_ptr<WrapSlider> decay_;
    std::unique_ptr<WrapSlider> sustain_;
    std::unique_ptr<WrapSlider> release_;
};
}