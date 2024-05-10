#pragma once

#include "engine/synth.h"
#include "engine/modulation/curve.h"
#include "ui/curve_editor.h"
#include "ui/WrapDropBox.h"
#include "ui/warp_text_input.h"

namespace mana {
class CurveLayout {
public:
    CurveLayout(CurveManager& manager);

    void Paint();
    void SetBounds(float x, float y, float w, float h);
private:
    CurveManager& manager_;
    CurveEditor curve_editor_;
    WrapDropBox curve_selector_;
    WrapTextBox expression_input_box_;
};
}