#include "warp_text_input.h"

namespace mana {
WrapTextBox::WrapTextBox() {
    text_buffer_.resize(kMaxChars);
    text_box_.SetText(text_buffer_.data());
    text_box_.SetTextSize(kMaxChars);
}

void WrapTextBox::Paint() {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        text_box_.SetEditMode(CheckCollisionPointRec(GetMousePosition(), text_box_.GetBounds().GetRectangle()));
    }

    if (text_box_.Show()) {
        on_enter_press(this);
    }
}
}