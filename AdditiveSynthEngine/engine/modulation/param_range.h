#pragma once

#include <cmath>

namespace mana {
struct ParamRange {
    inline constexpr float ConvertFrom01(float x) const {
        return std::lerp(vmin, vmax, x);
    }

    inline constexpr float ConvertTo01(float x) const {
        return (x - vmin) / (vmax - vmin);
    }

    constexpr ParamRange(float vmin, float vmax, float vdefault, float vstep) : vmin(vmin), vmax(vmax), vdefault(vdefault), vstep(vstep) {}
    constexpr ParamRange(float vmin, float vmax) : ParamRange(vmin, vmax, (vmin + vmax) / 2, 0.0f) {}
    constexpr ParamRange(float vmin, float vmax, float vdefault) : ParamRange(vmin, vmax, vdefault, 0.0f) {}

    float vmax;
    float vmin;
    float vdefault;
    float vstep;
};
}