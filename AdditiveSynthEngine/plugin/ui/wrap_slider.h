#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "engine/modulation/Parameter.h"

namespace mana {
class WrapSlider : public juce::Slider {
public:
    WrapSlider(FloatParameter* p);
    WrapSlider(IntParameter* p);
    ~WrapSlider() override;
private:
    std::unique_ptr<juce::SliderParameterAttachment> attachment_;
};
}
