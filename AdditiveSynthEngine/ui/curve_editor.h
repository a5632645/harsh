#pragma once

#include "engine/modulation/curve.h"
#include "raylib.h"

namespace mana {
class CurveEditor {
public:
    void Paint();
    void SetBounds(Rectangle bound);
    void SetCurrentCurve(CurveManager::Curve* pcurve) { current_curve_ = pcurve; }
    CurveManager::Curve& GetCurrentCurve() { return *current_curve_; }
private:
    void DoLogicCheck();
    void RenderCurve();

    CurveManager::Curve* current_curve_{};
    Rectangle bounds_{};
};
}