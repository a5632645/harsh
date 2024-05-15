#pragma once

#include "engine/EngineConfig.h"
#include <vector>
#include <unordered_map>
#include <format>
#include <ranges>

namespace mana {
class CurveManager {
public:
    struct Curve {
        constexpr explicit Curve(size_t size) : data(size) {}

        constexpr explicit Curve(std::initializer_list<float> init) : data(init) {}

        constexpr explicit Curve() = default;

        std::vector<float> data;
    };

    template<class... T>
    CurveManager& AddCurve(size_t size, std::format_string<T...> format_text, T&&... args) {
        curves_[std::format(format_text, std::forward<T>(args)...)] = Curve(size);
        return *this;
    }

    template<class... T>
    CurveManager& AddCurve(std::initializer_list<float> init, std::format_string<T...> format_text, T&&... args) {
        curves_[std::format(format_text, std::forward<T>(args)...)] = Curve(init);
        return *this;
    }

    template<class... T> requires (sizeof...(T) > 0)
        Curve& GetCurve(std::format_string<T...> format_text, T&&... args) { return curves_.at(std::format(format_text, std::forward<T>(args)...)); }
    Curve& GetCurve(std::string_view id) { return curves_.at(std::string{ id }); }

    template<class... T> requires (sizeof...(T) > 0)
        Curve* GetCurvePtr(std::format_string<T...> format_text, T&&... args) { return &curves_.at(std::format(format_text, std::forward<T>(args)...)); }
    Curve* GetCurvePtr(std::string_view id) { return &curves_.at(std::string{ id }); }

    auto GetCurveIds() const { return curves_ | std::ranges::views::keys; }
private:
    std::unordered_map<std::string, Curve> curves_;
};
}