#pragma once

#include <vector>
#include <ranges>
#include <unordered_map>
#include <memory>
#include <cassert>
#include "Parameter.h"

namespace mana {
class ParamBank {
public:
    template<IsParamter Type = FloatParameter>
    Type& AddOrCreateIfNull(std::string_view id) {
        auto e = std::make_unique<Type>(id);
        auto& v = *e;
        parameters_[std::string(id)] = std::move(e);
        return v;
    }

    template<IsParamter Type = FloatParameter>
    Type* GetParamPtr(std::string_view id) const {
        auto e = parameters_.at(std::string(id)).get();
        assert(e->GetParamType() == Type::kTypeEnum);
        assert(e != nullptr);
        return static_cast<Type*>(e);
    }
private:
    std::unordered_map<std::string, std::unique_ptr<FloatParameter>> parameters_;
};
}