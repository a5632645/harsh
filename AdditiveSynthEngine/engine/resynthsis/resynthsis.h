#pragma once

#include "engine/IProcessor.h"
#include "param/synth_param.h"
#include "resynthsis_data.h"

namespace mana {
class Synth;
}

namespace mana {
class Resynthesis : public IProcessor {
public:
    Resynthesis(Synth& s) : synth_(s) {}

    void Init(float sample_rate) override;
    void Process(Partials& partials) override;
    void OnUpdateTick(const SynthParam& param, int skip, int module_idx) override;
    void OnNoteOn(int note) override;
    void OnNoteOff() override { is_running_ = false; }

    void PreGetFreqDiffsInRatio(Partials& partials);
    float GetPlayerPosition() const { return frame_pos_; }
private:
    bool IsWork() const;
    std::array<float, kNumPartials> GetFormantGains(Partials& partials,
                                                    const ResynthsisFrames::FftFrame& frame) const;

    Synth& synth_;
    float sample_rate_{};
    float frame_pos_{};
    float frame_player_pos_{};

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