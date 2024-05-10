#include "wrap_check_box.h"

namespace mana {
void WrapCheckBox::Paint() {
    auto b = Show();
    if (parameter != nullptr) {
        parameter->SetBool(b);
    }
}
}