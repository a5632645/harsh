#include "Modulation.h"
#include "Parameter.h"

namespace mana {
Modulation::Modulation(std::shared_ptr<Modulator> modulator, std::shared_ptr<Parameter> param) :
    m_modulator(modulator),
    m_parameter(param) {
}

void Modulation::set_amount(float amount) {
    m_amount = amount;
}

void Modulation::set_bipolar(bool bipolar) {
    m_bipolar = bipolar;
}

float Modulation::get_output_value() const {
    auto v = m_modulator->get_output_value();
    if (!m_bipolar) {
        v = 0.5f * v + 0.5f;
    }
    return v * m_amount;
}

float Modulation::get_amount() const {
    return m_amount;
}

bool Modulation::get_bipolar() const {
    return m_bipolar;
}

Modulator const& Modulation::get_modulator() const {
    return *m_modulator;
}

Modulator& Modulation::get_modulator() {
    return *m_modulator;
}

Parameter const& Modulation::get_parameter() const {
    return *m_parameter;
}

Parameter& Modulation::get_parameter() {
    return *m_parameter;
}
}