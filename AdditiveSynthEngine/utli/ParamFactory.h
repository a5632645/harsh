#pragma once
#include <string_view>
#include <unordered_map>
#include "Value.hpp"
#include <memory>

namespace mana {
class ParamFactory {
public:
    static ParamFactory& getInstance() {
        static ParamFactory instance;
        return instance;
    }

    Value<float>& addParam(
        std::string name,
        float minValue,
        float maxValue,
        float default_alue
    ) {
        m_valueMap[name] = std::make_unique<Value<float>>(minValue, maxValue, default_alue, name);
        return *m_valueMap[name];
    }

    Value<float>& getParam(std::string name) {
        auto& p = m_valueMap[name];
        return *p;
    }

    Value<float>* getPointer(std::string name) {
        return m_valueMap[name].get();
    }

private:
    std::unordered_map<std::string, std::unique_ptr<Value<float>>> m_valueMap;
};
}