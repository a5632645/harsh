#pragma once

#include "ui/wrap_check_box.h"
#include "ui/wrap_drop_box.h"
#include "ui/wrap_slider.h"

namespace mana {
class ReverbSection : public juce::Component, private juce::ComboBox::Listener {
public:
    ReverbSection(Synth& s);
    void resized() override;
    void paint(juce::Graphics& g);
private:
    void SetShowFdnUi(bool show);
    void SetShowFreeverbUi(bool show);

    std::unique_ptr<WrapCheckBox> is_enable_;
    std::unique_ptr<juce::Label> title_;
    std::unique_ptr<WrapSlider> damp_;
    std::unique_ptr<WrapSlider> dry_;
    std::unique_ptr<WrapSlider> roomsize_;
    std::unique_ptr<WrapSlider> wet_;
    std::unique_ptr<WrapSlider> width_;

    std::unique_ptr<WrapSlider> distribution_;
    std::unique_ptr<WrapSlider> separate_;
    std::unique_ptr<WrapSlider> feedback2_;
    std::unique_ptr<WrapSlider> feedback3_;
    std::unique_ptr<WrapSlider> freq_shift_;
    std::unique_ptr<WrapDropBox> fdn_matrix_;
    std::unique_ptr<WrapDropBox> reverb_type_;

    // 通过 Listener 继承
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
};
}