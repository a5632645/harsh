#pragma once

#include <cassert>
#include <format>
#include <memory>
#include <string>
#include <tuple>
#include <string_view>
#include <unordered_map>
#include "param_range.h"
#include "Parameter.h"

namespace mana {
template<IsParamter... PT>
class TempParamBank {
public:
    //template<IsParamter Type = FloatParameter, class... T>
    //Type& AddOrCreateIfNull(ModulationType type,
    //                        ParamRange range,
    //                        std::string_view name,
    //                        std::format_string<T...> format_text, T&&...args) {
    //    auto& storer = GetParamMap<Type>();

    //    auto e = std::make_unique<Type>(type, std::move(range), name, format_text, std::forward<T>(args)...);
    //    auto& v = *e;
    //    storer[e->GetIdStringRef()] = std::move(e);
    //    return v;
    //}

    template<IsParamter Type>
    Type& AddParameter(std::unique_ptr<Type> param) {
        auto& storer = GetParamMap<Type>();

        assert(param != nullptr);
        auto str_id = param->GetId();
        assert(!storer.contains(str_id));
        auto& v = *param;
        storer[str_id] = std::move(param);
        return v;
    }

    template<IsParamter Type = FloatParameter, class... T>
    Type* GetParamPtr(std::format_string<T...> format_text, T&&...args) const {
        auto& storer = GetParamMap<Type>();

        auto e = storer.at(std::format(format_text, std::forward<T>(args)...)).get();
        assert(e != nullptr);
        return static_cast<Type*>(e);
    }

    template<IsParamter Type>
    using StoreType = std::unordered_map<std::string_view, std::unique_ptr<Type>>;

    template<IsParamter Type = FloatParameter>
    decltype(auto) GetParamMap() const {
        return std::get<StoreType<Type>>(parameters_);
    }

    template<IsParamter Type = FloatParameter>
    decltype(auto) GetParamMap() {
        return std::get<StoreType<Type>>(parameters_);
    }
private:
    std::tuple<StoreType<PT>...> parameters_;
};

using ParamBank = TempParamBank<FloatParameter, IntParameter, IntChoiceParameter, BoolParameter>;
}