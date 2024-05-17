#pragma once

#include "engine/oscillor_param.h"
#include "param/filter_param.h"
#include "engine/partials.h"

namespace mana {
class Filter {
public:
    void Init(float sample_rate, float update_rate);
    void Process(Partials & partials);
    void PrepareParams(OscillorParams& params);
    void OnUpdateTick();
    void OnNoteOn(int note);
    void OnNoteOff();
private:
    std::array<PolyModuFloatParameter*, 6> filter_args_;
    IntParameter* filter_type_arg_;
    param::Filter_Type::ParamEnum filter_type_;
    float sample_rate_;

    // ===============================================================
    // sub processor
    // ===============================================================
    void DoLowPassFilter(Partials& partials);
    void DoHighPassFilter(Partials& partials);
    void DoBandPassFilter(Partials& partials);
    void DoBandStopFilter(Partials& partials);
    void DoSmoothLowPassFilter(Partials& partials);
    float cutoff_semitone_;
    float cutoff_knee_;
    float normalized_cutoff_;
    float filter_width_;
    float resonance_;
    float resonance_width_;
    float slope_;

    // ===============================================================
    // comb filter
    // ===============================================================
    void SetCombCutoff(float cutoff, float pitch);
    void DoCombFilter(Partials& partials);

    float comb_cycles_;
    float comb_phaser_cycles_;
    float comb_shape_;
    float comb_phase_;
    float comb_phaser_;
    float comb_depth_;
    float key_track_;

    // ===============================================================
    // phaser filter
    // ===============================================================
    void DoPhaserFilter(Partials& partials);
    float phaser_cycles_;
    float phaser_shape_;
    float phaser_phase_;
    float phaser_depth_;
    float phaser_width_;
    float phaser_notches_;
    float phaser_begin_pitch_;
    float phaser_end_pitch_;
};
}