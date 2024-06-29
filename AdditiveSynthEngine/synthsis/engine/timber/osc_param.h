#pragma once

#include <array>
#include "engine/poly_param.h"

namespace mana {
struct OscParam {
    std::array<ModuFloatParameter*, 4> args;
};
}