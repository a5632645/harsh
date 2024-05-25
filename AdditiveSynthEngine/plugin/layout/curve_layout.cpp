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

    using namespace std::string_view_literals;
    curve_operator_.SetChoices(std::array{
        "zero"sv,
        "ramp up"sv,
        "random"sv,
        "ramp down"sv,
        "sine"sv,
                               });
    curve_operator_.on_choice_changed = [this](int op) {
        DoOperator(op, curve_editor_.GetCurrentCurve());
    };
}

void CurveLayout::Paint() {
    curve_editor_.Paint();
    curve_selector_.Paint();
    expression_input_box_.Paint();
    curve_operator_.Paint();
}

void CurveLayout::SetBounds(float x, float y, float w, float h) {
    constexpr auto top_menu_height = 16;
    curve_selector_.SetBounds(rgc::Bounds(x, y, 200.0f, top_menu_height));
    curve_operator_.SetBounds(rgc::Bounds(x, y + top_menu_height, 200.0f, top_menu_height));
    expression_input_box_.GetTextBox().SetBounds(rgc::Bounds(x + 200, y, w - 50, top_menu_height));
    curve_editor_.SetBounds({ x + 200,y + top_menu_height,w - 200,h - 16 });
}

void CurveLayout::DoOperator(int op, CurveManager::Curve& curve) {
    int data_size = static_cast<int>(curve.data.size());
    switch (op) {
    case 0:
        std::ranges::fill(curve.data, float{});
        break;
    case 1:
        std::ranges::copy(std::views::iota(0, data_size)
                          | std::views::transform([data_size](int i) {return i / (data_size - 1.0f); }),
                          curve.data.begin());
        break;
    case 2:
    {
        std::default_random_engine engine;
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        for (auto& v : curve.data) {
            v = dist(engine);
        }
    }
    break;
    case 3:
        std::ranges::copy(std::views::iota(0, data_size)
                          | std::views::transform([data_size](int i) {return 1.0f - i / (data_size - 1.0f); }),
                          curve.data.begin());
        break;
    case 4:
        std::ranges::copy(std::views::iota(0, data_size)
                          | std::views::transform([data_size](int i) {
            auto x = i / (data_size - 1.0f);
            return 0.5f + 0.5f * std::sin(x * std::numbers::pi_v<float> *2.0f);
        }),
                          curve.data.begin());
        break;
    default:
        break;
    }
}
}