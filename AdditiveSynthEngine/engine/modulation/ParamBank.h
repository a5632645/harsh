#pragma once

#include <cassert>
#include <format>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include "param_range.h"
#include "Parameter.h"

namespace mana {
class ParamBank {
public:
    template<IsParamter Type = FloatParameter>
    ParamBank& AddOrCreateIfNull(ModulationType type, ParamRange range, std::string_view id) {
        parameters_[std::string(id)] = std::make_unique<Type>(type, std::move(range), id);
        return *this;
    }

    template<IsParamter Type = FloatParameter, class... T> requires (sizeof...(T) >= 1)
        ParamBank& AddOrCreateIfNull(ModulationType type, ParamRange range, std::format_string<T...> format_text, T&&...args) {
        auto id = std::format(format_text, std::forward<T>(args)...);
        parameters_[id] = std::make_unique<Type>(type, range, id);
        return *this;
    }

    template<IsParamter Type = FloatParameter, class... T> requires (sizeof...(T) >= 1)
        Type* GetParamPtr(std::format_string<T...> format_text, T&&...args) const {
        auto e = parameters_.at(std::format(format_text, std::forward<T>(args)...)).get();
        assert(e->GetParamType() == Type::kTypeEnum);
        assert(e != nullptr);
        return static_cast<Type*>(e);
    }

    template<IsParamter Type = FloatParameter>
    Type* GetParamPtr(std::string_view id) const {
        auto e = parameters_.at(std::string(id)).get();
        assert(e->GetParamType() == Type::kTypeEnum);
        assert(e != nullptr);
        return static_cast<Type*>(e);
    }

    decltype(auto) GetParamsMap() { return (parameters_); }
private:
    std::unordered_map<std::string, std::unique_ptr<FloatParameter>> parameters_;
};
}