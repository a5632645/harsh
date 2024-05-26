#pragma once

#include <array>

namespace mana {
template<typename DetailParam>
void SetSingeKnobInfo(WrapSlider* knob, DetailParam) {
    SetSingeKnobInfo(*knob, DetailParam{});
}

template<typename DetailParam>
void SetSingeKnobInfo(const std::unique_ptr<WrapSlider>& knob, DetailParam) {
    SetSingeKnobInfo(*knob, DetailParam{});
}

template<typename DetailParam>
void SetSingeKnobInfo(WrapSlider& knob, DetailParam) {
    constexpr auto text_getter = DetailParam::GetText;
    constexpr auto param_title = DetailParam::kName;

    //knob.set_title(param_title);
    //knob.value_to_text_function = text_getter;
    //knob.SetDefaultValue(DetailParam::GetNormalDefault());
    //knob.SetEnable(true);

    knob.setTitle(juce::String{ param_title.data(), param_title.length() });
    knob.textFromValueFunction = [](double v)->juce::String {
        auto org_text = DetailParam::GetText(v);
        return juce::String{ org_text.data(), org_text.length() };
    };
    knob.setVisible(true);
}

template<std::ranges::random_access_range Array, typename... Params>
    requires requires(Array& arr) {
        { *arr[0] } -> std::same_as<WrapSlider&>;
}
void SetGuiKnobs(Array& knobs, Params... params) {
    auto set_gui_knob_info = [&](auto param) {
        using DetailParam = decltype(param);
        constexpr int arg_idx = DetailParam::kArgIdx;
        SetSingeKnobInfo(*knobs[arg_idx], param);
        return 0;
    };

    for (auto& knob : knobs) {
        //knob.SetEnable(false);
        knob->setVisible(false);
    }
    int use_less[]{ 0, set_gui_knob_info(params)... };
}
}