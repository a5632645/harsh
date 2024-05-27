#pragma once

#include <string>

namespace mana {
struct ModulationConfig {
    std::string modulator_id;
    std::string param_id;
    float amount{};
    bool enable{};
    bool bipolar{};
};
}