#pragma once

#include <string>
#include <vector>
#include <functional>
#include "raygui-cpp.h"

namespace mana {
class WrapDropBox : public rgc::DropdownBox {
public:
    WrapDropBox();

    void show();

    void set_text(std::string_view text);
    void set_text(const std::vector<std::string_view>& text);

    int get_item_selected() const { return m_item_selected; }

    std::function<void(int)> on_choice_changed = [](int) {};
private:
    std::string m_concated_string;
    bool m_is_edit_mode{};
    int m_item_selected{};
};
}