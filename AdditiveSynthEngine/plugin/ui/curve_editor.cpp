#include "curve_editor.h"

#include "utli/warp.h"

namespace mana {
void CurveEditor::Paint() {
    DrawRectangleLines(bounds_.x, bounds_.y, bounds_.width, bounds_.height, WHITE);

    if (current_curve_ == nullptr) {
        return;
    }

    DoLogicCheck();
    RenderCurve();
}

void CurveEditor::SetBounds(Rectangle bound) {
    bounds_ = bound;
}

void CurveEditor::DoLogicCheck() {
    if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        return;
    }

    auto mouse_pos = GetMousePosition();
    if (!CheckCollisionPointRec(mouse_pos, bounds_)) {
        return;
    }

    auto x_nor = (mouse_pos.x - bounds_.x) / bounds_.width;
    auto y_nor = 1.0f - ((mouse_pos.y - bounds_.y) / bounds_.height);
    utli::warp::AtNormalizeIndex(current_curve_->data, x_nor) = y_nor;
}

void CurveEditor::RenderCurve() {
    auto rec_width = bounds_.width / current_curve_->data.size();
    for (int x = 0; auto s : current_curve_->data) {
        auto rec_left = static_cast<int>(x * rec_width + bounds_.x);
        ++x;
        auto rec_right = static_cast<int>(x * rec_width + bounds_.x);
        auto rec_bottom = static_cast<int>(bounds_.y + bounds_.height);
        auto rec_top = static_cast<int>(bounds_.y + (1.0f - s) * bounds_.height);
        DrawRectangle(rec_left, rec_top, rec_right - rec_left, rec_bottom - rec_top, GREEN);
    }
}
}