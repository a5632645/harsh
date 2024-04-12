#pragma once
#include "raygui-cpp.h"

namespace mana {
class WrapGroupBox : public rgc::GroupBox {
public:
    rgc::Bounds get_bounds() const;
    void set_bounds(rgc::Bounds bounds);
};
}