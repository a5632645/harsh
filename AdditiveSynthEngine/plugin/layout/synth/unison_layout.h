#pragma once

#include "engine/forward_decalre.h"
#include "ui/wrap_slider.h"
#include "ui/wrap_drop_box.h"
#include "layout/modu_container.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace mana {
class UnisonLayout : public ModuContainer, public juce::Component {
public:
    UnisonLayout(SynthParams& params);

    void resized() override;

    // 通过 ModuContainer 继承
    void BeginHighlightModulator(std::string_view id) override;
    void StopHighliteModulator() override;
private:
    std::unique_ptr<WrapDropBox> type_;
    std::unique_ptr<WrapSlider> num_voice_;
    std::unique_ptr<WrapSlider> pitch_;
    std::unique_ptr<WrapSlider> phase_;
    std::unique_ptr<WrapSlider> pan_;


};
}