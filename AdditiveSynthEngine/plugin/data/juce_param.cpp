#include "juce_param.h"

namespace mana {
void JuceFloatParam::SetValue(float new_val) {
    *ref_ = new_val;
}

float JuceFloatParam::GetValue() const {
    return ref_->get();
}

void JuceFloatParam::Set01Value(float new_val) {
    ref_->setValueNotifyingHost(ref_->convertFrom0to1(new_val));
}

float JuceFloatParam::Get01Value() const {
    return ref_->convertTo0to1(ref_->get());
}

float JuceFloatParam::ConvertFrom01(float x) {
    return ref_->convertFrom0to1(x);
}

float JuceFloatParam::ConvertTo01(float x) {
    return ref_->convertTo0to1(x);
}

std::string_view JuceFloatParam::GetId() const {
    return std::string_view{ ref_->getParameterID().toRawUTF8(), static_cast<std::size_t>(ref_->getParameterID().length()) };
}

std::string_view JuceFloatParam::GetName() const {
    return std::string_view{ ref_->name.toRawUTF8(), static_cast<std::size_t>(ref_->name.length()) };
}

float JuceFloatParam::PraseValue(std::string_view text) {
    return 0.0f;
}

std::string JuceFloatParam::FormantValue(float v) {
    return std::string();
}

//========================================================================
// juce int param
//========================================================================
void JuceIntParam::SetValue(int new_val) {
    *ref_ = new_val;
}

int JuceIntParam::GetInt() const {
    return ref_->get();
}

void JuceIntParam::Set01Value(float new_val) {
    ref_->setValueNotifyingHost(new_val);
}

float JuceIntParam::Get01Value() const {
    return ref_->convertTo0to1(ref_->get());
}

int JuceIntParam::ConvertFrom01(float x) {
    return static_cast<int>(std::round(ref_->convertFrom0to1(x)));
}

float JuceIntParam::ConvertTo01(int x) {
    return ref_->convertTo0to1(static_cast<float>(x));
}

std::string_view JuceIntParam::GetId() const {
    return std::string_view{ ref_->getParameterID().toRawUTF8(), static_cast<std::size_t>(ref_->getParameterID().length()) };
}

std::string_view JuceIntParam::GetName() const {
    return std::string_view{ ref_->name.toRawUTF8(), static_cast<std::size_t>(ref_->name.length()) };
}

int JuceIntParam::PraseValue(std::string_view text) {
    return 0;
}

std::string JuceIntParam::FormantValue(int v) {
    return std::string();
}

//========================================================================
// juce bool param
//========================================================================
void JuceBoolParam::SetValue(bool new_val) {
    *ref_ = new_val;
}

bool JuceBoolParam::GetBool() const {
    return ref_->get();
}

std::string_view JuceBoolParam::GetId() const {
    return std::string_view{ ref_->getParameterID().toRawUTF8(), static_cast<std::size_t>(ref_->getParameterID().length()) };
}

std::string_view JuceBoolParam::GetName() const {
    return std::string_view{ ref_->name.toRawUTF8(), static_cast<std::size_t>(ref_->name.length()) };
}

bool JuceBoolParam::PraseValue(std::string_view text) {
    return 0;
}

std::string JuceBoolParam::FormantValue(bool v) {
    return std::string();
}

//========================================================================
// juce bool param
//========================================================================
void JuceEnumParam::SetValue(int new_val) {
    *ref_ = new_val;
}

int JuceEnumParam::GetInt() const {
    return *ref_;
}

void JuceEnumParam::Set01Value(float new_val) {
    ref_->setValueNotifyingHost(ref_->convertFrom0to1(new_val));
}

float JuceEnumParam::Get01Value() const {
    return ref_->convertTo0to1(static_cast<float>(*ref_));
}

int JuceEnumParam::ConvertFrom01(float x) {
    return static_cast<int>(std::round(ref_->convertFrom0to1(x)));
}

float JuceEnumParam::ConvertTo01(int x) {
    return ref_->convertTo0to1(static_cast<float>(x));
}

std::string_view JuceEnumParam::GetId() const {
    return std::string_view{ ref_->getParameterID().toRawUTF8(), static_cast<std::size_t>(ref_->getParameterID().length()) };
}

std::string_view JuceEnumParam::GetName() const {
    return std::string_view{ ref_->name.toRawUTF8(), static_cast<std::size_t>(ref_->name.length()) };
}

int JuceEnumParam::PraseValue(std::string_view text) {
    return 0;
}

std::string JuceEnumParam::FormantValue(int v) {
    return std::string();
}
}