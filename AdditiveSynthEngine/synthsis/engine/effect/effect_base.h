#pragma once

#include "engine/partials.h"
#include "engine/poly_param.h"
#include "engine/modulation/curve.h"
#include "engine/oscillor_param.h"

namespace mana {
struct EffectParams {
    std::array<PolyModuFloatParameter*, 6> args;
};
}

namespace mana {
class EffectBase {
public:
    EffectBase() = default;
    virtual ~EffectBase() = default;

    EffectBase(const EffectBase&) = default;
    EffectBase& operator=(const EffectBase&) = default;
    EffectBase(EffectBase&&) noexcept = default;
    EffectBase& operator=(EffectBase&&) noexcept = default;

    virtual void Init(float sample_rate, float update_rate) = 0;
    virtual void Process(Partials& partials) = 0;
    virtual void OnUpdateTick(EffectParams& args, CurveManager& curves) = 0;
    virtual void OnNoteOn(int note) = 0;
    virtual void OnNoteOff() = 0;
    virtual void PrepareParams(OscillorParams& params) {}
};
}