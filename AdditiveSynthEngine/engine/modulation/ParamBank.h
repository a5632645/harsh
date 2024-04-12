#pragma once

#include <vector>
#include <ranges>
#include <unordered_map>
#include <memory>
#include "Parameter.h"
#include "ChoiceParameter.h"

namespace mana {
class ParamBank {
public:
    void UpdateParamOutput();

    Parameter& AddOrCreateIfNull(std::string_view id,
                                 std::string_view name,
                                 float default_value,
                                 float min, float max, float step);

    Parameter& AddOrCreateIfNull(std::string_view id,
                                 std::string_view name,
                                 float default_value,
                                 float min, float max, int step_count);

    ChoiceParameter& AddOrCreateIfNull(std::string_view id,
                                       std::string_view name,
                                       std::vector<std::string_view> choices,
                                       std::string_view default_choice);

    ChoiceParameter& AddOrCreateIfNull(std::string_view id,
                                       std::string_view name,
                                       std::vector<std::string_view> choices,
                                       int default_choice);

    Parameter* GetParamPtr(std::string_view id) const;
private:
    std::unordered_map<std::string_view, std::shared_ptr<Parameter>> parameters_;
};
}