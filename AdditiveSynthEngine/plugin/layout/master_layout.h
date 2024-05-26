#pragma once

#include <array>
#include "engine/forward_decalre.h"
#include "ui/wrap_slider.h"
#include "ui/wrap_drop_box.h"
#include "ui/wrap_check_box.h"
//#include "ui/oscilloscope.h"
#include "ui/spectrum.h"
#include "layout/modu_container.h"

namespace mana {
class MasterLayout : public juce::Component, public ModuContainer {
public:
    MasterLayout(Synth& synth);

    void resized() override;

    //Oscilloscope& GetWaveScope() { return wave_scope_; }
private:
    Synth& synth_;

    std::unique_ptr<WrapSlider> pitch_bend_;
    std::unique_ptr<WrapSlider> output_gain_;
    //Oscilloscope wave_scope_std::unique_ptr<>;
    Spectrum spectrum_;
};
}