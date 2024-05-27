#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "engine/modulation/Parameter.h"

namespace mana {
class WrapSlider : public juce::Slider, public juce::DragAndDropTarget {
public:
    WrapSlider(FloatParameter* p);
    WrapSlider(IntParameter* p);
    ~WrapSlider() override;
private:
    juce::RangedAudioParameter& parameter_;
    std::unique_ptr<juce::SliderParameterAttachment> attachment_;

    // 通过 DragAndDropTarget 继承
    bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override;
    void itemDropped(const SourceDetails& dragSourceDetails) override;
};
}
