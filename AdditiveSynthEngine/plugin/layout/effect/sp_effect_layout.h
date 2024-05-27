#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>
#include "engine/forward_decalre.h"
#include "ui/wrap_check_box.h"
#include "ui/wrap_drop_box.h"
#include "ui/wrap_slider.h"
#include "layout/modu_container.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace mana {
class EffectLayout : public ModuContainer, public juce::Component, private juce::ComboBox::Listener {
public:
    EffectLayout(Synth& synth, int effect_idx);

    void resized() override;

    // 通过 ModuContainer 继承
    void BeginHighlightModulator(std::string_view id) override;
    void StopHighliteModulator() override;
private:
    void OnEffectTypeChanged(int c);

    const int effect_idx_;
    std::unique_ptr<WrapCheckBox> is_enable_;
    std::unique_ptr<WrapDropBox> effect_type_;
    std::vector<std::unique_ptr<WrapSlider>> arg_knobs_;

    // 通过 Listener 继承
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
};
}
