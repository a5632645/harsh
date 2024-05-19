#pragma once

#include <string>
#include <vector>
#include <functional>
#include <ranges>
#include <raygui-cpp/DropdownBox.h>
#include "engine/modulation/Parameter.h"
#include <cassert>

namespace mana {
class WrapDropBox : public rgc::DropdownBox {
public:
    WrapDropBox() { SetActive(&m_item_selected); }
    WrapDropBox(IntParameter* p) : WrapDropBox() { SetParameter(p); }
    WrapDropBox(IntChoiceParameter* p) : WrapDropBox() { SetParameter(p); }

    void SetParameter(IntChoiceParameter* p) {
        assert(p != nullptr);

        parameter_ = p;
        SetChoices(p->choices_);
    }

    void SetParameter(IntParameter* p) {
        assert(p != nullptr);

        int_parameter_ = p;
        auto vmax = static_cast<int>(p->GetRange().vmax);
        auto vmin = static_cast<int>(p->GetRange().vmin);
        SetChoices(std::views::iota(vmin, vmax + 1) | std::views::transform([](int x) {return std::to_string(x); }));
    }

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
    struct Adapter;

    IntChoiceParameter* parameter_{};
    IntParameter* int_parameter_{};
    std::string m_concated_string;
    std::vector<std::string> choice_strings_;
    bool m_is_edit_mode{};
    int m_item_selected{};
};
}