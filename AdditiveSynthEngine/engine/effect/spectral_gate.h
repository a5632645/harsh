#pragma once

#include <numbers>
#include <random>
#include <complex>
#include "effect_base.h"
#include "param/effect_param.h"
#include "param/param.h"
#include "utli/convert.h"

namespace mana {
class SpectralGate : public EffectBase {
public:
    void Init(float sample_rate, float update_rate) override {}

    void Process(Partials& partials) override {
        for (auto& l : partials.gains) {
            l = (l >= gate_level_ ? l : 0.0f);
        }
    }

    void OnUpdateTick(EffectParams& args, CurveManager& curves) override {
        gate_level_ = helper::GetAlterParamValue(args.args, param::SpectralGate_Level{});
    }

    void OnNoteOn(int note) override {}
    void OnNoteOff() override {}
private:
    PolyModuFloatParameter* arg_gate_level_{};
    float gate_level_{};
};
}