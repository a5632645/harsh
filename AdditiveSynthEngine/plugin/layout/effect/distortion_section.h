#pragma once

#include "ui/wrap_check_box.h"
#include "ui/wrap_drop_box.h"
#include "ui/wrap_slider.h"

namespace mana {
class DistortionSection : public juce::Component {
public:
    DistortionSection(Synth& s);
    void resized() override;
    void paint(juce::Graphics& g) override;
private:
    std::unique_ptr<juce::Label> title_;
    std::unique_ptr<WrapCheckBox> is_enable_;
    std::unique_ptr<WrapDropBox> distortion_type_;
    std::unique_ptr<WrapSlider> distortion_amount_;
    std::unique_ptr<WrapSlider> filter_mode_;
    std::unique_ptr<WrapDropBox> filter_type_;
    std::unique_ptr<WrapSlider> filter_cutoff_;
};
}