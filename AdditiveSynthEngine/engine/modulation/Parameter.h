#pragma once

#include <string>
#include <algorithm>
#include <string_view>
#include <memory>
#include <vector>
#include <functional>

namespace mana {
class Modulator;
class Modulation;
class Knob;

class Parameter {
public:
    Parameter(std::string_view id,
              float default_value);

    virtual ~Parameter() = default;
    Parameter(Parameter const&) = delete;
    Parameter(Parameter&&) = delete;
    Parameter& operator=(Parameter const&) = delete;
    Parameter& operator=(Parameter&&) = delete;

    float get_no_modulation_value() const {
        return m_value;
    }

    float get_output_value() const {
        return m_output_value;
    }

    void set_current(float v) {
        m_value = std::clamp(v, 0.0f, 1.0f);
    }

    std::string const& get_id() const {
        return m_id;
    }

    float get_default_value() const {
        return m_default_value;
    }

    void update_output();

    void add_modulation(std::shared_ptr<Modulation> m);
    void remove_modulation(std::string_view id);
    bool has_modulation() const {
        return !m_modulations.empty();
    }
    std::shared_ptr<Modulation> find_modulator(std::string_view modulator_id);
    std::function<void(float)> on_output_changed = [](float) {};
protected:
    virtual void _on_output_changed(float v) {}

    std::string m_id;
    float m_value{};
    float m_default_value{};
    float m_output_value{};
    std::vector<std::shared_ptr<Modulation>> m_modulations;
};
}