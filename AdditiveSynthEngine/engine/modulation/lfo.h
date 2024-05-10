#pragma once

#include "Modulator.h"

#include <random>
#include "engine/modulation/curve.h"
#include "param/lfo_param.h"

namespace mana {
class LFO : public Modulator {
public:
    LFO(std::string_view id, int idx) : Modulator(id), idx_(idx) {}

    void Init(float sample_rate, float update_rate);
    void Process(Partials& partials) {}
    void OnUpdateTick(const OscillorParams & params, int /*skip*/, int /*module_idx*/);
    void OnNoteOn(int note);
    void OnNoteOff() {}
private:
    const int idx_{};
    float inv_update_rate_{};
    float lfo_phase_{};
    float start_phase_{};
    bool restart_{};
    float output_level_{};
    float last_random_value_{};
    param::LFO_WaveType::ParamEnum wave_type_{};
    int wave_curve_idx_{};
    CurveManager::Curve* wave_curve_{};
    std::random_device random_;
    std::uniform_real_distribution<float> urd_;
};
}