#pragma once

#include <string>
#include <vector>
#include <functional>
#include <ranges>
#include "raygui-cpp.h"

namespace mana {
class WrapDropBox : public rgc::DropdownBox {
public:
    WrapDropBox();

    void show();

    void set_text(std::string_view text);
    void set_text(const std::vector<std::string_view>& text);

    template<std::ranges::input_range rng>
    void SetChoices(rng&& choices) {
        m_concated_string.clear();
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

    std::function<void(int)> on_choice_changed = [](int) {};
private:
    std::string m_concated_string;
    bool m_is_edit_mode{};
    int m_item_selected{};
};
}