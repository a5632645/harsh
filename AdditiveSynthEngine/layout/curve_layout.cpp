#include "curve_layout.h"

#include <ranges>
#include <numeric>
#include <vector>
#include <metl.hpp>

namespace mana {
CurveLayout::CurveLayout(CurveManager& manager)
    : manager_(manager) {
    using namespace std::ranges;
    curve_selector_.SetChoices(views::iota(0, kNumCurves)
                               | views::transform([](int x) {return std::to_string(x); }));

    curve_selector_.on_choice_changed = [this](int choice) {
        curve_editor_.SetCurrentCurve(manager_.GetCurvePtr(choice));
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
    curve_selector_.SetBounds(rgc::Bounds(x, y, 50.0f, top_menu_height));
    expression_input_box_.GetTextBox().SetBounds(rgc::Bounds(x + 50, y, w - 50, top_menu_height));
    curve_editor_.SetBounds({ x + 50,y + top_menu_height,w,h - 16 });
}
}