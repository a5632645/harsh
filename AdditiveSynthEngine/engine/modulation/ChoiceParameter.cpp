#include "ChoiceParameter.h"
#include "utli/DebugMarco.h"
#include "ui/ChooseKnob.h"

static size_t find_choice(std::string_view choice, 
                          std::vector<std::string_view> const& choices) {
    for (size_t i = 0; i < choices.size(); i++) {
        if (choices[i] == choice) {
            return i;
        }
    }
    MANA_ASSERT(false);
    return 0;
}

namespace mana {
ChoiceParameter::ChoiceParameter(std::string_view id,
                                 std::string_view name,
                                 std::vector<std::string_view> choices,
                                 std::string_view default_choice) :
    Parameter(id, name, find_choice(default_choice, choices), 0.0F, choices.size() - 1.0F, 1.0F),
    m_choices(choices) {
}

ChoiceParameter::ChoiceParameter(std::string_view id,
                                 std::string_view name,
                                 std::vector<std::string_view> choices,
                                 int default_choice)
    : Parameter(id, name, default_choice, 0.0f, choices.size() - 1.0f, 1.0f)
    , m_choices(choices) {
}

void ChoiceParameter::_on_output_changed(float v) {
    auto vv = static_cast<size_t>(v);
    on_choice_changed(vv);
}
}