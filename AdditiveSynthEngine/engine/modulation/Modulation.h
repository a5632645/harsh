#pragma once

namespace mana {
struct ModulationConfig {
    std::string_view modulator_id;
    std::string_view param_id;
    float amount{};
    bool enable{};
    bool bipolar{};
};
}