#include "quantize_map.h"

#include <nlohmann/json.hpp>

namespace mana {
nlohmann::json QuantizeMap::SaveState() const {
    // todo: implement
    return {};
}

void QuantizeMap::LoadState(const nlohmann::json& json) {
    // todo: implement
}
}