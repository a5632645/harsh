#include "curve_layout.h"

#include <ranges>
#include <numeric>
#include <vector>

namespace mana {
CurveLayout::CurveLayout(CurveManager& manager)
    : manager_(manager) {
    curve_selector_.SetChoices(manager.GetCurveIds());

    curve_selector_.on_choice_changed = [this](int choice) {
        curve_editor_.SetCurrentCurve(manager_.GetCurvePtr(curve_selector_.GetChoiceText(choice)));
    };
    curve_selector_.on_choice_changed(0);

    expression_input_box_.on_enter_press = [](WrapTextBox* box) {
        box->GetTextBuffer().clear();
    };
}

void CurveLayout::Paint() {
    curve_editor_.Paint();
    curve_selector_.Paint();
    expression_input_box_.Paint();
}

void CurveLayout::SetBounds(float x, float y, float w, float h) {
    constexpr auto top_menu_height = 16;
    curve_selector_.SetBounds(rgc::Bounds(x, y, 200.0f, top_menu_height));
    expression_input_box_.GetTextBox().SetBounds(rgc::Bounds(x + 200, y, w - 50, top_menu_height));
    curve_editor_.SetBounds({ x + 200,y + top_menu_height,w - 200,h - 16 });
}
}