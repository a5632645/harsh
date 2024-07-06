#pragma once

#include "ui/wrap_check_box.h"
#include "ui/wrap_drop_box.h"
#include "ui/wrap_slider.h"

namespace mana {
class ChrousSection : public juce::Component, private juce::ComboBox::Listener {
public:
    ChrousSection(Synth& s);
    void resized() override;
    void paint(juce::Graphics& g);
private:
    std::unique_ptr<WrapCheckBox> is_enable_;
    std::unique_ptr<juce::Label> title_;
    std::unique_ptr<WrapDropBox> rate_mode_;
    std::unique_ptr<WrapSlider> chrous_rate_;
    std::unique_ptr<WrapSlider> delay1_time_;
    std::unique_ptr<WrapSlider> delay2_time_;
    std::unique_ptr<WrapSlider> depth_;
    std::unique_ptr<WrapSlider> feedback_;

    // 通过 Listener 继承
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
};
}