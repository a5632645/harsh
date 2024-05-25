#include "juce_param_creator.h"

#include "juce_param.h"

namespace mana {
std::unique_ptr<FloatParameter> JuceParamCreator::CreateFloatParameter(FloatCreateParam p) {
    auto e = std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(p.id),
        p.name,
        p.vmin,
        p.vmax,
        p.vdefault
    );
    auto w = std::make_unique<JuceFloatParam>(e.get(), p.type);
    juce_params_.emplace_back(std::move(e));
    return w;
}

std::unique_ptr<BoolParameter> JuceParamCreator::CreateBoolParameter(BoolCreateParam p) {
    auto e = std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(p.id),
        p.name,
        p.vdefault
    );
    auto w = std::make_unique<JuceBoolParam>(e.get());
    juce_params_.emplace_back(std::move(e));
    return w;
}

std::unique_ptr<IntParameter> JuceParamCreator::CreateIntParameter(IntCreateParam p) {
    auto e = std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID(p.id),
        p.name,
        p.vmin,
        p.vmax,
        p.vdefault
    );
    auto w = std::make_unique<JuceIntParam>(e.get());
    juce_params_.emplace_back(std::move(e));
    return w;
}

std::unique_ptr<IntChoiceParameter> JuceParamCreator::CreateIntChoiceParameter(IntChoiceCreateParam p) {
    juce::StringArray juce_sa;
    for (auto& s : p.choices) {
        juce_sa.add(juce::String{ s.data(), s.size() });
    }

    auto e = std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(p.id),
        p.name,
        juce_sa,
        0
    );
    auto w = std::make_unique<JuceEnumParam>(e.get());
    juce_params_.emplace_back(std::move(e));
    return w;
}
}