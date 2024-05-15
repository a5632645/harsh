#pragma once

#include "engine/oscillor_param.h"
#include "resynthsis_data.h"
#include <engine/partials.h>
#include "engine/modulation/curve.h"

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
    void OnNoteOff() { is_running_ = false; }

    void PreGetFreqDiffsInRatio(Partials& partials);
    float GetPlayerPosition() const { return frame_pos_; }
private:
    bool IsWork() const;
    std::array<float, kNumPartials> GetFormantGains(Partials& partials,
                                                    const ResynthsisFrames::FftFrame& frame) const;
    std::array<PolyModuFloatParameter*, 7> args_{};
    BoolParameter* is_enable_arg_{};
    BoolParameter* is_formant_remap_{};
    CurveManager::Curve* formant_remap_curve_{};
    Synth& synth_;
    float sample_rate_{};
    float frame_pos_{};
    float frame_player_pos_{};
    int skip_{};

    // parameters
    bool is_running_{};
    bool is_enable_{};
    float formant_mix_{};
    float formant_shift_{};
    float freq_scale_{};
    float frame_offset_{};
    float frame_speed_{};
    float gain_mix_{};
};
}