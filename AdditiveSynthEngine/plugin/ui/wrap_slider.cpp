#include "wrap_slider.h"

#include "data/juce_param.h"

namespace mana {
WrapSlider::WrapSlider(FloatParameter * p)
    : parameter_(*p) {
    attachment_ = std::make_unique<juce::SliderParameterAttachment>(parameter_.As<JuceFloatParam>().GetRef(), *this);
}

WrapSlider::~WrapSlider() {
    attachment_ = nullptr;
}
}