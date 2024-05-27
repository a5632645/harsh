#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "engine/forward_decalre.h"
#include "engine/synth_params.h"

namespace mana {
class ModulatorButton : public juce::Component
    , public SynthParams::ModulationListener {
public:
    ModulatorButton(std::string_view modulator_id, Synth& synth);
    ~ModulatorButton() override;

    void resized() override;
private:
    friend class WrapSlider;
    Synth& synth_;

    int num_modulator_{};
    std::string modulator_id_;
    std::unique_ptr<juce::Label> num_modulation_label_;
    std::unique_ptr<juce::Component> drag_component_;

    // 通过 ModulationListener 继承
    void OnModulationAdded(std::shared_ptr<ModulationConfig> config) override;
    void OnModulationRemoved(std::string_view modulator_id, std::string_view param_id) override;
};
}