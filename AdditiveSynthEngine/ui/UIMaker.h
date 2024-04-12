#pragma once

#include "engine/modulation/Parameter.h"
#include "Knob.h"

namespace mana {
class UIMaker {
public:
    static std::shared_ptr<Knob> MakeKnob(Parameter* paramter);
};
}