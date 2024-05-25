#pragma once

#include <string>
#include <vector>
#include <functional>
#include <ranges>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "engine/modulation/Parameter.h"
#include <cassert>

namespace mana {
class WrapDropBox : public juce::ComboBox {
public:
    WrapDropBox(IntChoiceParameter* p);
    ~WrapDropBox() override;
private:
    IntChoiceParameter& parameter_;
    std::unique_ptr<juce::ComboBoxParameterAttachment> attachment_;
};
}