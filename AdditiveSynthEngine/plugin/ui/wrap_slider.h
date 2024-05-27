#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "engine/modulation/Parameter.h"
#include "engine/forward_decalre.h"
#include "layout/modu_listener.h"
#include "engine/synth_params.h"

namespace mana {
class WrapSlider : public juce::Slider
    , public juce::DragAndDropTarget
    , public ModulationActionListener
    , public SynthParams::ModulationListener {
public:
    struct ParamRefStore;

    WrapSlider(FloatParameter* p);
    WrapSlider(IntParameter* p);
    ~WrapSlider() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void BeginHighlightModulator(std::string_view id) override;
    void StopHighliteModulator() override;
private:
    bool shound_highlight_ = false;
    Synth* synth_{};
    std::unique_ptr<ParamRefStore> ref_store_;

    juce::RangedAudioParameter& parameter_;
    std::unique_ptr<juce::SliderParameterAttachment> attachment_;

    // 通过 DragAndDropTarget 继承
    bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override;
    void itemDropped(const SourceDetails& dragSourceDetails) override;

    // 通过 ModulationListener 继承
    void OnModulationAdded(std::shared_ptr<ModulationConfig> config) override;
    void OnModulationRemoved(std::string_view modulator_id, std::string_view param_id) override;
};
}
