#pragma once

#include <array>
#include "engine/forward_decalre.h"
#include "ui/wrap_drop_box.h"
#include "ui/wrap_slider.h"
#include "layout/modu_container.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace mana {
class OscLayout : public ModuContainer, public juce::Component, private juce::ComboBox::Listener {
public:
    OscLayout(Synth& synth, int idx);

    void resized() override;

    // 通过 ModuContainer 继承
    void BeginHighlightModulator(std::string_view id) override;
    void StopHighliteModulator() override;
private:
    void OnTimberTypeChanged(int c);
    const int idx_;
    std::unique_ptr<WrapDropBox> timber_type_;
    std::array<std::unique_ptr<WrapSlider>, 4> arg_knobs_;

    // 通过 Listener 继承
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
};
}
