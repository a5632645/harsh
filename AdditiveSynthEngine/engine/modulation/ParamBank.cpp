#include <ranges>
#include <algorithm>
#include "ParamBank.h"

namespace mana {
void ParamBank::UpdateParamOutput() {
    for (auto&&[key, val] : parameters_) {
        val->update_output();
    }
}

Parameter& ParamBank::AddOrCreateIfNull(std::string_view id,
                                        float default_value) {
    auto e = std::make_shared<Parameter>(id, default_value);
    parameters_[std::string(id)] = e;
    return *e;
}

Parameter* ParamBank::GetParamPtr(std::string_view id) const {
    return parameters_.at(std::string(id)).get();
}
}