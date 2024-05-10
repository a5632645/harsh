#pragma once

#include <raygui-cpp.h>
#include <string>
#include <functional>

namespace mana {
class WrapTextBox {
public:
    WrapTextBox();

    void Paint();
    rgc::TextBox& GetTextBox() { return text_box_; }
    std::string& GetTextBuffer() { return text_buffer_; }

    std::function<void(WrapTextBox* box)> on_enter_press = [](auto) {};
private:
    static constexpr auto kMaxChars = 256;
    rgc::TextBox text_box_;
    std::string text_buffer_;
};
}