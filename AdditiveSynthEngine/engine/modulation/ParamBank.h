#pragma once

#include <vector>
#include <ranges>
#include <unordered_map>
#include <memory>
#include <cassert>
#include <format>
#include "Parameter.h"

namespace mana {
class ParamBank {
public:
    template<IsParamter Type = FloatParameter>
    Type& AddOrCreateIfNull(ModulationType type, std::string_view id) {
        auto e = std::make_unique<Type>(type, id);
        auto& v = *e;
        parameters_[std::string(id)] = std::move(e);
        return v;
    }

    template<IsParamter Type = FloatParameter, class... T> requires (sizeof...(T) >= 1)
        Type& AddOrCreateIfNull(ModulationType type, std::format_string<T...> format_text, T&&...args) {
        auto id = std::format(format_text, std::forward<T>(args)...);
        auto e = std::make_unique<Type>(type, id);
        auto& v = *e;
        parameters_[id] = std::move(e);
        return v;
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