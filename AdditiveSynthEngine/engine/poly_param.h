#pragma once

#include <algorithm>
#include "modulation/Parameter.h"

namespace mana {
struct PolyModuFloatParameter {
    PolyModuFloatParameter(FloatParameter* ref) : synth_param_ref(ref) {}

    FloatParameter* synth_param_ref;
    float ouput_value{};
    float modulation_value{};

    float GetClamp() const { return std::clamp(ouput_value + modulation_value, 0.0f, 1.0f); }
};
}