#pragma once

#include <array>
#include "engine/forward_decalre.h"
#include "ui/wrap_slider.h"
#include "ui/wrap_drop_box.h"
#include "ui/wrap_check_box.h"
#include "layout/modu_container.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace mana {
class DissonanceLayout : public ModuContainer, public juce::Component, private juce::ComboBox::Listener {
public:
    DissonanceLayout(Synth& synth);

    void resized() override;

    // 通过 ModuContainer 继承
    void BeginHighlightModulator(std::string_view id) override;
    void StopHighliteModulator() override;
private:
    void OnDissonanceTypeChanged(int c);
    std::unique_ptr<WrapCheckBox> is_enable_;
    std::unique_ptr<WrapDropBox> type_;
    std::array<std::unique_ptr<WrapSlider>, 2> arg_knobs_;

    // 通过 Listener 继承
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
};
}
