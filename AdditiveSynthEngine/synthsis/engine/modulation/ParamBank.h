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
    template<IsParamter... Type> requires (sizeof...(Type) > 1)
        void AddParameter(std::unique_ptr<Type>... param) {
        (AddParameter<Type>(std::move(param)), ...);
    }

    template<IsParamter Type>
    void AddParameter(std::unique_ptr<Type> param) {
        auto& storer = GetParamMap<Type>();

        assert(param != nullptr);
        auto str_id = param->GetId();
        assert(!storer.contains(str_id));
        auto& v = *param;
        storer[str_id] = std::move(param);
    }

    template<IsParamter Type = FloatParameter, class... T> requires (sizeof...(T) >= 1)
        Type* GetParamPtr(std::format_string<T...> format_text, T&&...args) const {
        return GetParamPtr<Type>(std::format(format_text, std::forward<T>(args)...));
    }

    template<IsParamter Type = FloatParameter>
    Type* GetParamPtr(std::string_view id) const {
        auto& storer = GetParamMap<Type>();
        auto e = storer.at(id).get();
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