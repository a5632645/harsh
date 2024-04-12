#pragma once

#include <string_view>

namespace mana {
class ParamSection;

class Modulator {
public:
    Modulator(std::string_view id,
              std::string_view name);

    virtual ~Modulator() = default;
    Modulator(Modulator const&) = delete;
    Modulator(Modulator&&) = delete;
    Modulator& operator=(Modulator const&) = delete;
    Modulator& operator=(Modulator&&) = delete;

    virtual void Init(float sample_rate) = 0;
    virtual void update_state(size_t step) = 0;
    virtual void build_parameters(ParamSection& bank) = 0;
    virtual void note_on() = 0;

    float get_output_value() const {
        return m_output_value;
    }

    std::string_view get_id() const {
        return m_id;
    }

protected:
    std::string with_id(std::string_view x) const {
        return std::string(m_id) + "_" + std::string(x);
    }

    std::string_view m_id;
    std::string_view m_name;
    float m_output_value{};
};
}