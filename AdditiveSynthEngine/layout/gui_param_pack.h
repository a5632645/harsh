#pragma once

#include <array>

namespace mana {
namespace detail {
template<class Array, typename E>
constexpr void SetGuiParamPackInfo(Array& arr, E) {
    using DetailParam = typename E::TargetParam;
    arr[DetailParam::kArgIdx].title = DetailParam::kName;
    arr[DetailParam::kArgIdx].text_getter = E::GetText;
}

template<class Array, typename CurrentParam, typename... Params>
constexpr void SetGuiParamPackInfo(Array& arr, CurrentParam, Params... params) {
    using DetailParam = typename CurrentParam::TargetParam;
    arr[DetailParam::kArgIdx].title = DetailParam::kName;
    arr[DetailParam::kArgIdx].text_getter = CurrentParam::GetText;
    SetGuiParamPackInfo(arr, params...);
}
}

template<typename... Params>
constexpr decltype(auto) CreateGuiParamPack(Params... params) {
    struct GuiParamInfo {
        using TextGetterFunc = std::string(*)(float);

        std::string_view title;
        TextGetterFunc text_getter;
    };

    constexpr auto num_params = sizeof...(Params);
    std::array<GuiParamInfo, num_params> out;
    detail::SetGuiParamPackInfo(out, params...);
    return out;
}

template<std::ranges::random_access_range Array, typename... Params>
    requires std::same_as < std::ranges::range_value_t<Array>, Knob>
void SetGuiKnobs(Array& knobs, Params... params) {
    constexpr auto infos = CreateGuiParamPack(params...);
    for (int i = 0; auto && info : infos) {
        knobs[i].set_title(info.title);
        knobs[i].value_to_text_function = info.text_getter;
        knobs[i].SetEnable(true);
        ++i;
    }
    for (size_t i = std::size(infos); i < std::size(knobs); ++i) {
        knobs[i].SetEnable(false);
    }
}
}