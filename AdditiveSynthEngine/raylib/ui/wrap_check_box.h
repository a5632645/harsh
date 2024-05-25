#pragma once

#include <string>
#include <vector>
#include <functional>
#include <ranges>
#include "raygui-cpp.h"
#include "engine/modulation/Parameter.h"

namespace mana {
class WrapCheckBox : public rgc::CheckBox {
public:
    WrapCheckBox(BoolParameter* p = nullptr) : parameter(p) {}
    void SetParameter(BoolParameter* p) {
        parameter = p;
        SetText(p->GetName().data());
    }
    void Paint();
private:
    BoolParameter* parameter;
};
}