#pragma once

#include <array>
#include "engine/forward_decalre.h"
#include "ui/wrap_slider.h"
#include "ui/wrap_drop_box.h"
#include "ui/wrap_check_box.h"
#include "layout/modu_container.h"

namespace mana {
class LfoLayout : public juce::Component, public ModuContainer {
public:
    LfoLayout(Synth& synth, int idx);
    
    void resized() override;

    // 通过 ModuContainer 继承
    void BeginHighlightModulator(std::string_view id) override;
    void StopHighliteModulator() override;
private:
    std::unique_ptr<WrapSlider> rate_;
    std::unique_ptr<WrapSlider> start_phase_;
    std::unique_ptr<WrapSlider> level_;
    std::unique_ptr<WrapCheckBox> restart_;
    std::unique_ptr<WrapDropBox> wave_type_;
};
}