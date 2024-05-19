#pragma once

#include <algorithm>
#include <cassert>
#include "modulation/Parameter.h"

namespace mana {
struct PolyModuFloatParameter {
    PolyModuFloatParameter(FloatParameter* ref) : synth_param_ref(ref) { assert(ref != nullptr); }

    // synth param ref
    FloatParameter* synth_param_ref;
    // modulation value are 01 normalized
    float modulation_value{};

    float GetValue() const {
        return synth_param_ref->GetRange().ConvertFrom01(Get01Value());
    }

    float Get01Value() const {
        auto nor_v = synth_param_ref->Get01Value() + modulation_value;
        return std::clamp(nor_v, 0.0f, 1.0f);
    }

    float GetValueNoModu() const {
        return synth_param_ref->GetValue();
    }

    float Get01ValueNoModu() const {
        return synth_param_ref->Get01Value();
    }
};
}