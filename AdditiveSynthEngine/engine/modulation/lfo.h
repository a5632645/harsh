#pragma once

#include "Modulator.h"

#include <random>
#include "engine/modulation/curve.h"
#include "param/lfo_param.h"
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
    PolyModuFloatParameter* arg_start_phase_{};
    PolyModuFloatParameter* arg_lfo_rate_{};
    BoolParameter* restart_{};
    PolyModuFloatParameter* arg_output_level_{};
    IntChoiceParameter* arg_wave_type_{};

    const int idx_{};
    float inv_update_rate_{};
    float lfo_phase_{};
    float last_random_value_{};
    float start_phase_{};
    param::LFO_WaveType::ParamEnum wave_type_{};
    CurveManager::Curve* wave_curve_{};
    std::random_device random_;
    std::uniform_real_distribution<float> urd_;
};
}