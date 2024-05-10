#pragma once

#include <random>
#include "engine/IProcessor.h"
#include "param/dissonance_param.h"

namespace mana {
class Dissonance : public IProcessor {
public:
    void Init(float sample_rate, float update_rate) override;
    void Process(Partials & partials) override;
    void OnUpdateTick(const OscillorParams& params, int skip, int module_idx) override;
    void OnNoteOn(int note) override;
    void OnNoteOff() override;
private:
    bool is_enable_;
    param::DissonanceType::ParamEnum type_;

    // string
    float string_stretch_factor_;

    // harm stretch
    float harmonic_stretch_ratio_;

    // st space
    float st_space_semitone_;

    // static noise
    void DoSyncNoise(Partials& partials);
    std::random_device random_;
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