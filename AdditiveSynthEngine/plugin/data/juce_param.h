#pragma once

#include "engine/modulation/Parameter.h"
#include <juce_audio_processors/juce_audio_processors.h>

namespace mana {
class JuceFloatParam : public FloatParameter {
public:
    JuceFloatParam(juce::AudioParameterFloat* ptr,
                   ModulationType t)
        : ref_(ptr)
        , FloatParameter(t) {
        jassert(ptr != nullptr);
    }

    // 通过 FloatParameter 继承
    void SetValue(float new_val) override;
    float GetValue() const override;
    void Set01Value(float new_val) override;
    float Get01Value() const override;
    float ConvertFrom01(float x) override;
    float ConvertTo01(float x) override;
    std::string_view GetId() const override;
    std::string_view GetName() const override;
    float PraseValue(std::string_view text) override;
    std::string FormantValue(float v) override;
private:
    juce::AudioParameterFloat* ref_;
};

class JuceIntParam : public IntParameter {
public:
    JuceIntParam(juce::AudioParameterInt* ptr)
        : ref_(ptr) {
        jassert(ptr != nullptr);
    }

    // 通过 IntParameter 继承
    void SetValue(int new_val) override;
    int GetInt() const override;
    void Set01Value(float new_val) override;
    float Get01Value() const override;
    int ConvertFrom01(float x) override;
    float ConvertTo01(int x) override;
    std::string_view GetId() const override;
    std::string_view GetName() const override;
    int PraseValue(std::string_view text) override;
    std::string FormantValue(int v) override;
private:
    juce::AudioParameterInt* ref_;
};

class JuceBoolParam : public BoolParameter {
public:
    JuceBoolParam(juce::AudioParameterBool* ptr)
        : ref_(ptr) {
        jassert(ptr != nullptr);
    }

    // 通过 BoolParameter 继承
    void SetValue(bool new_val) override;
    bool GetBool() const override;
    std::string_view GetId() const override;
    std::string_view GetName() const override;
    bool PraseValue(std::string_view text) override;
    std::string FormantValue(bool v) override;
private:
    juce::AudioParameterBool* ref_;
};

class JuceEnumParam : public IntChoiceParameter {
public:
    JuceEnumParam(juce::AudioParameterChoice* ptr)
        : ref_(ptr) {
        jassert(ptr != nullptr);
    }

    // 通过 IntChoiceParameter 继承
    void SetValue(int new_val) override;
    int GetInt() const override;
    void Set01Value(float new_val) override;
    float Get01Value() const override;
    int ConvertFrom01(float x) override;
    float ConvertTo01(int x) override;
    std::string_view GetId() const override;
    std::string_view GetName() const override;
    int PraseValue(std::string_view text) override;
    std::string FormantValue(int v) override;
private:
    juce::AudioParameterChoice* ref_;
};
}