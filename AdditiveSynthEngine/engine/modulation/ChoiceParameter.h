#pragma once

#include "Parameter.h"

namespace mana {

class ChoiceParameter : public Parameter {
public:
    ChoiceParameter(std::string_view id,
                    std::string_view name,
                    std::vector<std::string_view> choices,
                    std::string_view default_choice);
    ChoiceParameter(std::string_view id,
                    std::string_view name,
                    std::vector<std::string_view> choices,
                    int default_choice);
    ~ChoiceParameter() override = default;
    ChoiceParameter(ChoiceParameter const&) = delete;
    ChoiceParameter(ChoiceParameter&&) = delete;
    ChoiceParameter& operator=(ChoiceParameter const&) = delete;
    ChoiceParameter& operator=(ChoiceParameter&&) = delete;

    std::vector<std::string_view> const& get_choices() const {
        return m_choices;
    }

    Type GetParamType() const override { return Type::kChoice; }

    std::function<void(size_t)> on_choice_changed = empty_choice_callback;
private:
    static void empty_choice_callback(size_t) { }
    void _on_output_changed(float v) override;

    std::vector<std::string_view> m_choices;
};

}