#pragma once

#include <string>
#include <string_view>
#include <concepts>
#include <atomic>
#include <format>
#include "param_range.h"
#include <functional>

namespace mana {
enum class ModulationType {
    kDisable,
    kMono,
    kPoly
};

class FloatParameter {
public:
    FloatParameter(ModulationType t) : modulation_type_(t) {}

    virtual ~FloatParameter() = default;
    FloatParameter(FloatParameter const&) = delete;
    FloatParameter& operator=(FloatParameter const&) = delete;
    FloatParameter(FloatParameter&&) noexcept = default;
    FloatParameter& operator=(FloatParameter&&) noexcept = default;

    virtual void SetValue(float new_val) = 0;
    virtual float GetValue() const = 0;

    virtual void Set01Value(float new_val) = 0;
    virtual float Get01Value() const = 0;

    virtual float ConvertFrom01(float x) = 0;
    virtual float ConvertTo01(float x) = 0;

    virtual std::string_view GetId() const = 0;
    virtual std::string_view GetName() const = 0;

    virtual float PraseValue(std::string_view text) = 0;
    virtual std::string FormantValue(float v) = 0;

    ModulationType GetModulationType() const { return modulation_type_; }

    template<typename T>
    requires std::derived_from<T, FloatParameter>
    T& As() { return static_cast<T&>(*this); }
protected:
    ModulationType modulation_type_;
};

class IntParameter {
public:
    IntParameter() = default;
    virtual ~IntParameter() = default;
    IntParameter(IntParameter const&) = delete;
    IntParameter& operator=(IntParameter const&) = delete;
    IntParameter(IntParameter&&) noexcept = default;
    IntParameter& operator=(IntParameter&&) noexcept = default;

    virtual void SetValue(int new_val) = 0;
    virtual int GetInt() const = 0;

    virtual void Set01Value(float new_val) = 0;
    virtual float Get01Value() const = 0;

    virtual int ConvertFrom01(float x) = 0;
    virtual float ConvertTo01(int x) = 0;

    virtual std::string_view GetId() const = 0;
    virtual std::string_view GetName() const = 0;

    virtual int PraseValue(std::string_view text) = 0;
    virtual std::string FormantValue(int v) = 0;

    template<typename T>
    requires std::derived_from<T, IntParameter>
    T& As() { return static_cast<T&>(*this); }
};

class IntChoiceParameter {
public:
    IntChoiceParameter() = default;
    virtual ~IntChoiceParameter() = default;
    IntChoiceParameter(IntChoiceParameter const&) = delete;
    IntChoiceParameter& operator=(IntChoiceParameter const&) = delete;
    IntChoiceParameter(IntChoiceParameter&&) noexcept = default;
    IntChoiceParameter& operator=(IntChoiceParameter&&) noexcept = default;

    virtual void SetValue(int new_val) = 0;
    virtual int GetInt() const = 0;

    virtual void Set01Value(float new_val) = 0;
    virtual float Get01Value() const = 0;

    virtual int ConvertFrom01(float x) = 0;
    virtual float ConvertTo01(int x) = 0;

    virtual std::string_view GetId() const = 0;
    virtual std::string_view GetName() const = 0;

    virtual int PraseValue(std::string_view text) = 0;
    virtual std::string FormantValue(int v) = 0;

    template<typename T>
    requires std::derived_from<T, IntChoiceParameter>
    T& As() { return static_cast<T&>(*this); }
};

class BoolParameter {
public:
    BoolParameter() = default;
    virtual ~BoolParameter() = default;
    BoolParameter(BoolParameter const&) = delete;
    BoolParameter& operator=(BoolParameter const&) = delete;
    BoolParameter(BoolParameter&&) noexcept = default;
    BoolParameter& operator=(BoolParameter&&) noexcept = default;

    virtual void SetValue(bool new_val) = 0;
    virtual bool GetBool() const = 0;

    virtual std::string_view GetId() const = 0;
    virtual std::string_view GetName() const = 0;

    virtual bool PraseValue(std::string_view text) = 0;
    virtual std::string FormantValue(bool v) = 0;

    template<typename T>
    requires std::derived_from<T, BoolParameter>
    T& As() { return static_cast<T&>(*this); }
};

template<class Type>
concept IsParamter = std::same_as<Type, FloatParameter> || std::same_as<Type, IntChoiceParameter> || std::same_as<Type, BoolParameter> || std::same_as<Type, IntParameter>;
}