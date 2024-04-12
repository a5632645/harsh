#include <ranges>
#include <algorithm>
#include "ParamBank.h"
#include "utli/DebugMarco.h"

namespace mana {
void ParamBank::UpdateParamOutput() {
    for (auto&&[key, val] : parameters_) {
        val->update_output();
    }
}

Parameter& ParamBank::AddOrCreateIfNull(std::string_view id,
                                        std::string_view name,
                                        float default_value,
                                        float min, float max, float step) {
    auto e = std::make_shared<Parameter>(id, name, default_value, min, max, step);
    parameters_[id] = e;
    return *e;
}

Parameter & ParamBank::AddOrCreateIfNull(std::string_view id, 
                                         std::string_view name, 
                                         float default_value, 
                                         float min, 
                                         float max, 
                                         int step_count) {
    return AddOrCreateIfNull(id, name, default_value, min, max, (max - min) / step_count);
}

ChoiceParameter& ParamBank::AddOrCreateIfNull(std::string_view id,
                                              std::string_view name,
                                              std::vector<std::string_view> choices,
                                              std::string_view default_choice) {
    auto x = std::make_shared<ChoiceParameter>(id, name, choices, default_choice);
    parameters_[id] = x;
    return *x;
}

ChoiceParameter& ParamBank::AddOrCreateIfNull(std::string_view id,
                                              std::string_view name,
                                              std::vector<std::string_view> choices,
                                              int default_choice) {
    auto x = std::make_shared<ChoiceParameter>(id, name, choices, default_choice);
    parameters_[id] = x;
    return *x;
}

Parameter* ParamBank::GetParamPtr(std::string_view id) const {
    return parameters_.at(id).get();
}
}