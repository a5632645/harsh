#pragma once

#include <vector>
#include <ranges>
#include <unordered_map>
#include <memory>
#include "Parameter.h"

namespace mana {
class ParamBank {
public:
    void UpdateParamOutput();

    Parameter& AddOrCreateIfNull(std::string_view id,
                                 float default_value);

    Parameter* GetParamPtr(std::string_view id) const;
private:
    std::unordered_map<std::string, std::shared_ptr<Parameter>> parameters_;
};
}