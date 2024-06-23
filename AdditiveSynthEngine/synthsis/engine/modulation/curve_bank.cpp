#include "curve_bank.h"

#include <nlohmann/json.hpp>

namespace mana {
nlohmann::json CurveBank::SaveState() const {
    nlohmann::json out;
    for (const auto&[id, map] : quantize_maps_) {
        out.push_back(nlohmann::json{
            {"type", "quantize_map"},
            {"id", id},
            {"value", map.SaveState()}
                      });
    }
    for (const auto&[id, curve] : curves_) {
        out.push_back(nlohmann::json{
            {"type", "curve"},
            {"id", id},
            {"value", curve.SaveState()}
                      });
    }
    return out;
}

void CurveBank::LoadState(const nlohmann::json& json) {
    using namespace std::string_literals;

    for (const auto& j : json) {
        auto type = j["type"].get<std::string>();
        auto id = j["id"].get<std::string>();
        if (type == "quantize_map"s) {
            quantize_maps_[id].LoadState(j["value"]);
        }
        else if (type == "curve"s) {
            curves_[id].LoadState(j["value"]);
        }
    }
}
}