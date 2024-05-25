#pragma once

#include <vector>
#include <juce_audio_processors/juce_audio_processors.h>
#include "engine/modulation/param_creator.h"

namespace mana {
class JuceParamCreator : public ParamCreator {
public:
    // 通过 ParamCreator 继承
    std::unique_ptr<FloatParameter> CreateFloatParameter(FloatCreateParam p) override;
    std::unique_ptr<BoolParameter> CreateBoolParameter(BoolCreateParam p) override;
    std::unique_ptr<IntParameter> CreateIntParameter(IntCreateParam p) override;
    std::unique_ptr<IntChoiceParameter> CreateIntChoiceParameter(IntChoiceCreateParam p) override;

    decltype(auto) MoveJuceParams() { return std::move(juce_params_); }
private:
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> juce_params_;
};
}