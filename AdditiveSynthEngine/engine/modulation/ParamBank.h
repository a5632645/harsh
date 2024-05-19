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
    /*template<IsParamter Type = FloatParameter>
    Type& AddOrCreateIfNull(ModulationType type, ParamRange range, std::string_view id) {
        auto e = std::make_unique<Type>(type, std::move(range), id);
        auto& v = *e;
        parameters_[std::string(id)] = std::move(e);
        return v;
    }*/

    template<IsParamter Type = FloatParameter, class... T>/* requires (sizeof...(T) >= 1)*/
    Type& AddOrCreateIfNull(ModulationType type,
                            ParamRange range,
                            std::string_view name,
                            std::format_string<T...> format_text, T&&...args) {
        auto e = std::make_unique<Type>(type, std::move(range), name, format_text, std::forward<T>(args)...);
        auto& v = *e;
        parameters_[e->GetIdStringRef()] = std::move(e);
        return v;
    }

    template<IsParamter Type>
    void AddParameter(std::unique_ptr<Type> param) {
        assert(param != nullptr);
        const auto& str_id = param->GetIdStringRef();
        assert(!parameters_.contains(str_id));
        parameters_[str_id] = std::move(param);
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