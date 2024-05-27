#include "wrap_slider.h"

#include "data/juce_param.h"
#include "layout/modulators/modulator_button.h"
#include "engine/synth.h"

namespace mana {
WrapSlider::WrapSlider(FloatParameter* p)
    : juce::Slider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox)
    , parameter_(p->As<JuceFloatParam>().GetRef()) {
    attachment_ = std::make_unique<juce::SliderParameterAttachment>(p->As<JuceFloatParam>().GetRef(), *this);
    setPopupDisplayEnabled(true, true, nullptr);
}

WrapSlider::WrapSlider(IntParameter * p)
    : juce::Slider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox)
    , parameter_(p->As<JuceIntParam>().GetRef()) {
    attachment_ = std::make_unique<juce::SliderParameterAttachment>(p->As<JuceIntParam>().GetRef(), *this);
    setPopupDisplayEnabled(true, true, nullptr);
}

WrapSlider::~WrapSlider() {
    attachment_ = nullptr;
}

bool WrapSlider::isInterestedInDragSource(const SourceDetails& dragSourceDetails) {
    return true;
}

void WrapSlider::itemDropped(const SourceDetails& dragSourceDetails) {
    auto modulator_id = dragSourceDetails.description.toString().toStdString();
    auto source_button_flag_component_ptr = dragSourceDetails.sourceComponent;
    if (source_button_flag_component_ptr.wasObjectDeleted()) {
        return;
    }

    // get the component that has the synth ref
    auto source_button_flag_component = source_button_flag_component_ptr.get();
    auto* ref_component = static_cast<ModulatorButton*>(source_button_flag_component->getParentComponent());
    auto& synth = ref_component->synth_;
    auto param_id = parameter_.paramID.toStdString();
    synth.CreateModulation(modulator_id, param_id);
}
}