#include "WrapGroupBox.h"

namespace mana {
constexpr auto padding = 5;

rgc::Bounds WrapGroupBox::get_bounds() const {
    auto b = GetBounds();
    return rgc::Bounds(b.GetX() + padding,
                       b.GetY() + padding,
                       b.GetWidth() - padding * 2,
                       b.GetHeight() - padding * 2);
}

void WrapGroupBox::set_bounds(rgc::Bounds bounds) {
    SetBounds(rgc::Bounds(bounds.GetX() - padding,
                          bounds.GetY() - padding,
                          bounds.GetWidth() + padding * 2,
                          bounds.GetHeight() + padding * 2));
}
}