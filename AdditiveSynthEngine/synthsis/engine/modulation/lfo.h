#pragma once

#include "Modulator.h"

#include <random>
#include "engine/modulation/curve_v2.h"
#include "engine/oscillor_param.h"

namespace mana {
class LFO : public Modulator {
public:
    LFO(std::string_view id, int idx) : Modulator(id), idx_(idx) {}

    void Init(float sample_rate, float update_rate) override;
    void PrepareParams(OscillorParams& params) override;
    void OnUpdateTick() override;
    void OnNoteOn(int note) override;
    void OnNoteOff() override {}
private:
    IntChoiceParameter* lfo_mode_{};
    IntChoiceParameter* lfo_rate_mode_{};
    IntChoiceParameter* wave_type_{};
    FloatParameter* bpm_{};
    PolyModuFloatParameter* lfo_rate_{};
    PolyModuFloatParameter* start_phase_{};
    CurveV2* wave_curve_{};

    const int idx_{};
    float inv_update_rate_{};
    float lfo_phase_{};
};
}