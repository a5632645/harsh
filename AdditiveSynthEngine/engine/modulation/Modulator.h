#pragma once

#include <string_view>
#include "engine/IProcessor.h"

namespace mana {
class Modulator : public IProcessor {
public:
    Modulator(std::string_view id) : id_(id) {}

    float get_output_value() const { return output_value_; }

    std::string_view get_id() const { return id_; }

protected:
    std::string with_id(std::string_view x) const {
        return std::string(id_) + "_" + std::string(x);
    }

    std::string id_;
    float output_value_{};
};
}