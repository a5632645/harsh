#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "engine/modulation/Parameter.h"

namespace mana {
class WrapSlider : public juce::Slider, public juce::DragAndDropTarget {
public:
    struct ParamRefStore;

    WrapSlider(FloatParameter* p);
    WrapSlider(IntParameter* p);
    ~WrapSlider() override;

    void paint(juce::Graphics& g) override;

    void BeginHighlightModulator(std::string_view id);
    void StopHighliteModulator();
private:
    bool shound_highlight_ = false;
    std::unique_ptr<ParamRefStore> ref_store_;

    juce::RangedAudioParameter& parameter_;
    std::unique_ptr<juce::SliderParameterAttachment> attachment_;

    // 通过 DragAndDropTarget 继承
    bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override;
    void itemDropped(const SourceDetails& dragSourceDetails) override;
};
}
