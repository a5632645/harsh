#pragma once

#include <array>

namespace mana {
template<std::ranges::random_access_range Array, typename... Params>
    requires std::same_as <std::ranges::range_value_t<Array>, Knob>
void SetGuiKnobs(Array& knobs, Params... params) {
    auto set_gui_knob_info = [&](auto param) {
        using DetailParam = decltype(param);

        constexpr int arg_idx = DetailParam::kArgIdx;
        constexpr auto text_getter = DetailParam::GetText;
        constexpr auto param_title = DetailParam::kName;

        auto& knob = knobs[arg_idx];
        knob.set_title(param_title);
        knob.value_to_text_function = text_getter;
        knob.SetDefaultValue(DetailParam::GetNormalDefault());
        knob.SetEnable(true);
        return 0;
    };

    for (auto& knob : knobs) {
        knob.SetEnable(false);
    }
    int use_less[]{ set_gui_knob_info(params)... };
}
}