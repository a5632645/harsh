#include <algorithm>
#include "Parameter.h"
#include "Modulation.h"
#include "ui/Knob.h"

namespace mana {
Parameter::Parameter(std::string_view id,
                     float default_value) :
    m_id(id),
    m_default_value(default_value),
    m_value(default_value) {
}

void Parameter::update_output() {
    float modulation_v = 0.0F;
    for (auto const& ptr_modulation : m_modulations) {
        modulation_v += ptr_modulation->get_output_value();
    }

    float v = modulation_v + m_value;
    v = std::clamp(v, 0.0f, 1.0f);

    if (v != m_output_value) {
        m_output_value = v;
        _on_output_changed(v);
        on_output_changed(v);
    }
}

void Parameter::add_modulation(std::shared_ptr<Modulation> m) {
    m_modulations.emplace_back(m);
}

void Parameter::remove_modulation(std::string_view modulator_id) {
    std::erase_if(m_modulations, [modulator_id](std::shared_ptr<Modulation> const& m) {
        return m->get_modulator().get_id() == modulator_id;
    });
}

std::shared_ptr<Modulation> Parameter::find_modulator(std::string_view modulator_id) {
    auto it = std::ranges::find_if(m_modulations, [modulator_id](std::shared_ptr<Modulation> const& p) {
        return modulator_id == p->get_modulator().get_id();
    });

    if (it == m_modulations.end()) {
        return {};
    }

    return *it;
}
}