#pragma once

#include <array>
#include "engine/forward_decalre.h"
#include "ui/wrap_slider.h"
#include "ui/wrap_drop_box.h"
#include "ui/wrap_check_box.h"

namespace mana {
class LfoLayout : public juce::Component {
public:
    LfoLayout(Synth& synth, int idx);
    
    void resized() override;
private:
    std::unique_ptr<WrapSlider> rate_;
    std::unique_ptr<WrapSlider> start_phase_;
    std::unique_ptr<WrapSlider> level_;
    std::unique_ptr<WrapCheckBox> restart_;
    std::unique_ptr<WrapDropBox> wave_type_;
};
}