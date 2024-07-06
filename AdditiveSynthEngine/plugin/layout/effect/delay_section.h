#pragma once

#include "ui/wrap_check_box.h"
#include "ui/wrap_drop_box.h"
#include "ui/wrap_slider.h"

namespace mana {
class DelaySection : public juce::Component, private juce::ComboBox::Listener {
public:
    DelaySection(Synth& s);
    void resized() override;
    void paint(juce::Graphics& g);
private:
    std::unique_ptr<WrapCheckBox> is_enable_;
    std::unique_ptr<juce::Label> title_;
    std::unique_ptr<WrapDropBox> time_mode_;
    std::unique_ptr<WrapSlider> delay_time_;
    std::unique_ptr<WrapSlider> feedback_;
    std::unique_ptr<WrapDropBox> delay_mode_;
    std::unique_ptr<WrapSlider> feedback_center_;
    std::unique_ptr<WrapSlider> feedback_bw_;

    // 通过 Listener 继承
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
};
}