#pragma once

#include <random>
#include "param/standard_param.h"
#include "engine/oscillor_param.h"
#include "engine/partials.h"

namespace mana {
class PhaseProcessor {
public:
    void Init(float sample_rate, float update_rate);
    void OnUpdateTick();
    void PrepareParams(OscillorParams& params);
    void OnNoteOn(int note);
    void OnNoteOff();
    void Process(Partials& partials);
private:
    inline void DoResetOnce(Partials& partials);
    inline void DoDispersion(Partials& partials);
    inline void DoSpectralRandom(Partials& partials);
    inline void DoPowDistribute(Partials& partials);

    bool note_on_once_flag_{};
    std::default_random_engine rand_;

    std::array<float, kNumPartials> random_phases_{};
    IntChoiceParameter* type_;
    param::PhaseType::ParamEnum process_type_{};
    PolyModuFloatParameter* arg0_{};
    PolyModuFloatParameter* arg1_{};
    float process_arg0_{};
    float process_arg1_{};
};
}