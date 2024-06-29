#pragma once

#include <array>
#include "engine/forward_decalre.h"
#include "ui/wrap_slider.h"
#include "ui/wrap_drop_box.h"

namespace mana {
class LfoLayout : public juce::Component, public juce::ComboBox::Listener {
public:
    LfoLayout(Synth& synth, int idx);

    void resized() override;
private:
    std::unique_ptr<WrapSlider> rate_;
    std::unique_ptr<WrapSlider> start_phase_;
    std::unique_ptr<WrapDropBox> wave_type_;
    std::unique_ptr<WrapDropBox> mode_type_;
    std::unique_ptr<WrapDropBox> time_type_;

    // 通过 Listener 继承
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
};
}