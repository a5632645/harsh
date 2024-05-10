#include "WrapDropBox.h"

void mana::WrapDropBox::Paint() {
    int old_choice = m_item_selected;

    SetEditMode(m_is_edit_mode);
    if (Show()) {
        if (m_is_edit_mode) {
            // close it
            m_is_edit_mode = false;
            if (old_choice != m_item_selected) {
                on_choice_changed(m_item_selected);
                if (parameter != nullptr) {
                    parameter->SetInt(m_item_selected);
                }
            }
        }
        else {
            // open it
            m_is_edit_mode = true;
        }
    }
}

void mana::WrapDropBox::set_text(std::string_view text) {
    m_concated_string = text;
    SetText(m_concated_string.c_str());
}

void mana::WrapDropBox::set_text(const std::vector<std::string_view>& text) {
    m_concated_string.clear();
    for (const auto sv : text) {
        m_concated_string += sv;
        m_concated_string += ';';
    }
    if (!m_concated_string.empty()) {
        m_concated_string.pop_back();
    }
    SetText(m_concated_string.c_str());
}