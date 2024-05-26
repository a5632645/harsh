#include "wrap_slider.h"

#include "data/juce_param.h"

namespace mana {
WrapSlider::WrapSlider(FloatParameter* p)
    : juce::Slider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox) {
    attachment_ = std::make_unique<juce::SliderParameterAttachment>(p->As<JuceFloatParam>().GetRef(), *this);
    setPopupDisplayEnabled(true, true, nullptr);
}

WrapSlider::WrapSlider(IntParameter * p)
    : juce::Slider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox) {
    attachment_ = std::make_unique<juce::SliderParameterAttachment>(p->As<JuceIntParam>().GetRef(), *this);
    setPopupDisplayEnabled(true, true, nullptr);
}

WrapSlider::~WrapSlider() {
    attachment_ = nullptr;
}
}