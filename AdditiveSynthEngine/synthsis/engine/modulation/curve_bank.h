#pragma once

#include <vector>
#include <unordered_map>
#include <format>
#include <ranges>
#include "quantize_map.h"
#include "curve_v2.h"

namespace mana {
class CurveBank {
public:
    template<class... T>
    CurveBank& AddCurve(std::format_string<T...> format_text, T&&... args) {
        curves_[std::format(format_text, std::forward<T>(args)...)] = CurveV2{};
        return *this;
    }
    template<class... T>
    CurveV2& GetCurve(std::format_string<T...> format_text, T&&... args) { return curves_.at(std::format(format_text, std::forward<T>(args)...)); }
    template<class... T> requires (sizeof...(T) > 0)
        CurveV2* GetCurvePtr(std::format_string<T...> format_text, T&&... args) { return GetCurvePtr(std::format(format_text, std::forward<T>(args)...)); }
    CurveV2* GetCurvePtr(const std::string& id) { return &curves_.at(id); }

    template<class... T>
    CurveBank& AddQuantizeMap(QuantizeMap map, std::format_string<T...> format_text, T&&... args) {
        quantize_maps_[std::format(format_text, std::forward<T>(args)...)] = std::move(map);
        return *this;
    }
    template<class... T>
    QuantizeMap& GetQuantizeMap(std::format_string<T...> format_text, T&&... args) { return quantize_maps_.at(std::format(format_text, std::forward<T>(args)...)); }
    template<class... T>
    QuantizeMap* GetQuantizeMapPtr(std::format_string<T...> format_text, T&&... args) { return &quantize_maps_.at(std::format(format_text, std::forward<T>(args)...)); }

    auto GetCurveIds() const { return curves_ | std::ranges::views::keys; }
    auto GetQuantizeMapIds() const { return quantize_maps_ | std::ranges::views::keys; }
private:
    std::unordered_map<std::string, QuantizeMap> quantize_maps_;
    std::unordered_map<std::string, CurveV2> curves_;
};
}