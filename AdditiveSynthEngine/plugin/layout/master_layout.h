#pragma once

#include <array>
#include "engine/forward_decalre.h"
#include "ui/wrap_slider.h"
#include "ui/wrap_drop_box.h"
#include "ui/wrap_check_box.h"
//#include "ui/oscilloscope.h"
#include "ui/spectrum.h"

namespace mana {
class MasterLayout : public juce::Component {
public:
    MasterLayout(Synth& synth);

    void resized() override;

    //Oscilloscope& GetWaveScope() { return wave_scope_; }
    void SetInfoLabelText(const juce::String& t) { info_label_->setText(t, juce::dontSendNotification); }
private:
    Synth& synth_;

    std::unique_ptr<WrapSlider> pitch_bend_;
    std::unique_ptr<WrapSlider> output_gain_;
    std::unique_ptr<juce::Label> info_label_;
    //Oscilloscope wave_scope_std::unique_ptr<>;
    Spectrum spectrum_;
};
}