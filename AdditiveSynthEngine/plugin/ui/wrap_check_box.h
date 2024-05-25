#pragma once

#include <string>
#include <vector>
#include <functional>
#include <ranges>
#include "engine/modulation/Parameter.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

namespace mana {
class WrapCheckBox : public juce::ToggleButton {
public:
    WrapCheckBox(BoolParameter* p);
    ~WrapCheckBox() override;
private:
    std::unique_ptr<juce::ButtonParameterAttachment> attachment_;
    BoolParameter& parameter_;
};
}