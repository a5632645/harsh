#pragma once

#include "engine/IProcessor.h"
#include "param/filter_param.h"

namespace mana {
class Filter : public IProcessor {
public:
    // 通过 IProcessor 继承
    void Init(float sample_rate) override;
    void Process(Partials & partials) override;
    void OnUpdateTick(const SynthParam& params, int skip) override;
    void OnNoteOn(int note) override;
    void OnNoteOff() override;
private:
    param::Filter_Type::TypeEnum filter_type_;
    float sample_rate_;

    // ===============================================================
    // sub processor
    // ===============================================================
    float cutoff_semitone_;
    float normalized_cutoff_;
    float resonance_;
    float slope_;
    float key_track_;
    float final_cutoff_;
    float final_cutoff_semitone_;

    // ===============================================================
    // comb filter
    // ===============================================================
    std::array<float, kNumPartials> latched_gains_{};
    void SetCombCutoff(float cutoff, float pitch);
    void DoCombFilter(Partials& partials);

    float comb_cycles_;
    float comb_phaser_cycles_;
    float comb_shape_;
    float comb_phase_;
    float comb_phaser_;
    float comb_depth_;

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