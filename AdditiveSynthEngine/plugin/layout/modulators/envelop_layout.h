#pragma once

#include "engine/forward_decalre.h"
#include "ui/wrap_slider.h"

namespace mana {
class EnvelopLayout : public juce::Component {
public:
    EnvelopLayout(Synth& synth, int idx);
    
    void resized() override;
private:
    std::unique_ptr<WrapSlider> predelay_;
    std::unique_ptr<WrapSlider> attack_;
    std::unique_ptr<WrapSlider> hold_;
    std::unique_ptr<WrapSlider> decay_;
    std::unique_ptr<WrapSlider> sustain_;
    std::unique_ptr<WrapSlider> release_;
    std::unique_ptr<WrapSlider> peak_;
    std::unique_ptr<WrapSlider> att_exp_;
    std::unique_ptr<WrapSlider> dec_exp_;
    std::unique_ptr<WrapSlider> rel_exp_;
};
}