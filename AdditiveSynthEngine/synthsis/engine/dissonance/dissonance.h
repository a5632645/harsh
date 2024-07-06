#pragma once

#include <random>
#include "engine/oscillor_param.h"
#include "engine/partials.h"
#include "engine/modulation/quantize_map.h"

namespace mana {
class Dissonance {
public:
    void Init(float sample_rate, float update_rate);
    void PrepareParams(ModulableParams& params);
    void Process(Partials& partials);
    void OnUpdateTick();
    void OnNoteOn(int note);
    void OnNoteOff();
private:
    void DoPitchQuantize(Partials& partials);
    void DoHarmonicStretch(Partials& partials);
    void DoSemitoneSpace(Partials& partials);
    void DoStringDiss(Partials& partials);
    void DoPrism(Partials& partials);
    void DoDispersion(Partials& partials);
    void DoSyncNoise(Partials& partials);
    void DoFakeUnison(Partials& partials);
    void DoFakeUnison2(Partials& partials);

    BoolParameter* is_enable_{};
    IntChoiceParameter* diss_type_{};
    std::array<ModuFloatParameter*, 2>args_{};
    QuantizeMap* pitch_quantize_map_{};
    CurveV2* prism_map_{};

    std::default_random_engine random_;
    std::uniform_real_distribution<float> random_dist_;
    std::array<float, kNumPartials> static_noise_{};
};
}