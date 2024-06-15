#pragma once

#include <random>
#include "engine/oscillor_param.h"
#include "resynthsis_data.h"
#include "engine/partials.h"
#include "engine/modulation/curve_v2.h"

namespace mana {
class Synth;
}

namespace mana {
class Resynthesis {
public:
    Resynthesis(Synth& s) : synth_(s) {}

    void Init(float sample_rate, float update_rate);
    void Process(Partials& partials);
    void OnUpdateTick();
    void PrepareParams(OscillorParams& params);
    void OnNoteOn(int note);
    void OnNoteOff() {}

    void PreGetFreqDiffsInRatio(Partials& partials);
    //float GetPlayerPosition() const { return frame_pos_; }
    decltype(auto) GetPartialPositions() const { return partials_frame_idxs_; }
private:
    bool IsWork() const;
    std::array<float, kNumPartials> GetFormantGains(Partials& partials) const;

    using R = struct { float gain_db; float ratio_diff; };
    R GetFrameGain(int partial_idx, int harmonic_idx) const;

    std::default_random_engine random_;
    std::array<float, kNumPartials> partials_init_frame_idxs_{};
    std::array<float, kNumPartials> partials_runnig_frame_idxs_{};
    std::array<float, kNumPartials> partials_frame_idxs_{};

    PolyModuFloatParameter* freq_scale_{};
    PolyModuFloatParameter* nor_start_pos_{};
    FloatParameter* partial_start_range_second_{};
    PolyModuFloatParameter* speed_{};
    PolyModuFloatParameter* speedx_{};
    PolyModuFloatParameter* formant_mix_{};
    PolyModuFloatParameter* formant_shift_{};
    PolyModuFloatParameter* gain_mix_{};

    BoolParameter* is_enable_{};
    BoolParameter* is_formant_remap_{};
    CurveV2* formant_remap_curve_{};
    CurveV2* speed_curve_{};
    CurveV2* pos_offset_curve_{};
    Synth& synth_;
    float sample_rate_{};
    float inv_sample_rate_{};
    float update_skip_{};
    float final_speed_{};
};
}