#pragma once

#include "engine/EngineConfig.h"
#include <array>
#include <vector>

namespace mana {
static constexpr auto kCurveSize = kNumPartials;
class CurveManager {
public:
    struct Curve {
        std::array<float, kCurveSize> data;
    };

    CurveManager() {
        curves_.resize(kNumCurves);
    }

    Curve& GetCurve(int idx) { return curves_[idx]; }
    Curve* GetCurvePtr(int idx) { return curves_.data() + idx; }

private:
    std::vector<Curve> curves_;
};
}