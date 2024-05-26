#pragma once

#include <array>
#include "ui/wrap_drop_box.h"
#include "ui/wrap_slider.h"
#include "layout/modu_container.h"
#include "engine/forward_decalre.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace mana {
class FilterLayout : public ModuContainer, public juce::Component, private juce::ComboBox::Listener, private juce::Slider::Listener {
public:
    FilterLayout(Synth& synth, int idx);

    void resized();
private:
    void OnResonanceTypeChanged(int c);
    void OnFilterTypeChanged(int c);
    std::unique_ptr<WrapDropBox> filter_type_;
    std::array<std::unique_ptr<WrapSlider>, 8> arg_filter_knobs_;

    int last_resonance_type_ = 0;

    // 通过 Listener 继承
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

    // 通过 Listener 继承
    void sliderValueChanged(juce::Slider* slider) override;
};
}
