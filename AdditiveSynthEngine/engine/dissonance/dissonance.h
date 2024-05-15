#pragma once

#include <random>
#include "engine/oscillor_param.h"
#include "engine/partials.h"

namespace mana {
class Dissonance {
public:
    void Init(float sample_rate, float update_rate);
    void PrepareParams(OscillorParams& params);
    void Process(Partials & partials);
    void OnUpdateTick();
    void OnNoteOn(int note);
    void OnNoteOff();
private:
    void DoPitchQuantize(Partials& partials);

    BoolParameter* is_enable_param_{};
    IntParameter* diss_type_{};
    std::array<PolyModuFloatParameter*, 2>args_{};
    CurveManager::Curve* pitch_quantize_map_{};

    bool is_enable_;

    // string
    float string_stretch_factor_;

    // harm stretch
    float harmonic_stretch_ratio_;

    // st space
    float st_space_semitone_;

    // static noise
    void DoSyncNoise(Partials& partials);
    std::default_random_engine random_;
    std::uniform_real_distribution<float> random_dist_;
    std::array<float, kNumPartials> static_noise_{};
    float error_range_;
    float error_ramp_;

    // fake unison
    float ratio2x_ratio_;
    float ratio3x_ratio_;

    // dispersion
    float dispersion_warp_;
    float dispersion_amount_;
};
}