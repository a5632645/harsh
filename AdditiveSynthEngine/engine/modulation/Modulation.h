#pragma once

#include <memory>
#include "Modulator.h"

namespace mana {
class Parameter;

class Modulation {
public:
    Modulation(std::shared_ptr<Modulator> modulator, std::shared_ptr<Parameter> param);
    ~Modulation() = default;
    Modulation(Modulation const&) = delete;
    Modulation(Modulation&&) = delete;
    Modulation& operator=(Modulation const&) = delete;
    Modulation& operator=(Modulation&&) = delete;

    void set_amount(float amount);
    void set_bipolar(bool bipolar);
    float get_output_value() const;
    float get_amount() const;
    bool get_bipolar() const;
    Modulator const& get_modulator() const;
    Modulator& get_modulator();
    Parameter const& get_parameter() const;
    Parameter& get_parameter();
private:
    std::shared_ptr<Parameter> m_parameter;
    std::shared_ptr<Modulator> m_modulator;
    float m_amount{};
    bool m_bipolar{};
};
}