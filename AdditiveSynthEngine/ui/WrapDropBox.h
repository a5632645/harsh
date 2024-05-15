#pragma once

#include <string>
#include <vector>
#include <functional>
#include <ranges>
#include <raygui-cpp/DropdownBox.h>
#include "engine/modulation/Parameter.h"

namespace mana {
class WrapDropBox : public rgc::DropdownBox {
public:
    WrapDropBox(IntParameter* p = nullptr) : parameter(p) { SetActive(&m_item_selected); }

    void SetParameter(IntParameter* p) { parameter = p; }

    void Paint();

    void set_text(std::string_view text);
    void set_text(const std::vector<std::string_view>& text);

    template<std::ranges::input_range rng>
        requires std::same_as<std::decay_t<std::ranges::range_value_t<rng>>, std::string>
    || std::same_as<std::decay_t<std::ranges::range_value_t<rng>>, std::string_view>
        void SetChoices(rng&& choices) {
        m_concated_string.clear();
        
        for (auto&& c : choices) {
            choice_strings_.emplace_back(c);
        }

        for (const auto& sv : choices) {
            m_concated_string += sv;
            m_concated_string += ';';
        }
        if (!m_concated_string.empty()) {
            m_concated_string.pop_back();
        }
        SetText(m_concated_string.c_str());
    }

    int get_item_selected() const { return m_item_selected; }
    std::string_view GetChoiceText(int c) const { return choice_strings_.at(c); }

    std::function<void(int)> on_choice_changed = [](int) {};
private:
    IntParameter* parameter;
    std::string m_concated_string;
    std::vector<std::string> choice_strings_;
    bool m_is_edit_mode{};
    int m_item_selected{};
};
}