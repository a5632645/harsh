#pragma once

#include <string>
#include <string_view>
#include <concepts>
#include <atomic>

namespace mana {
class FloatParameter {
public:
    enum class ParamType {
        kFloat,
        kInt,
        kBool
    };

    static constexpr auto kTypeEnum = ParamType::kFloat;

    FloatParameter(std::string_view id) : id_(id) {}

    virtual ~FloatParameter() = default;
    FloatParameter(FloatParameter const&) = default;
    FloatParameter(FloatParameter&&) = default;
    FloatParameter& operator=(FloatParameter const&) = default;
    FloatParameter& operator=(FloatParameter&&) = default;

    float Get() const { return value_; }
    void Set(float new_val) {
        if (new_val != value_) {
            value_ = new_val;
        }
    }

    virtual ParamType GetParamType() const { return ParamType::kFloat; }
protected:
    std::string id_;
    std::atomic<float> value_{};
};

class IntParameter : public FloatParameter {
public:
    static constexpr auto kTypeEnum = ParamType::kInt;

    using FloatParameter::FloatParameter;

    void SetInt(int v) { Set(static_cast<float>(v)); }
    int GetInt() const { return static_cast<int>(std::round(value_)); }
    operator int() const { return GetInt(); }

    ParamType GetParamType() const override { return ParamType::kInt; }
};

class BoolParameter : public FloatParameter {
public:
    static constexpr auto kTypeEnum = ParamType::kBool;

    using FloatParameter::FloatParameter;

    void SetBool(bool v) { Set(value_ = v ? 1.0f : 0.0f); }
    bool GetBool() const { return value_ > 0.5f; }
    operator bool() const { return GetBool(); }

    ParamType GetParamType() const override { return ParamType::kBool; }
};

template<class Type>
concept IsParamter = std::same_as<Type, FloatParameter> || std::same_as<Type, IntParameter> || std::same_as<Type, BoolParameter>;
}