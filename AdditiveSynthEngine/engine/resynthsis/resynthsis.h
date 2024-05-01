#pragma once

#include "engine/IProcessor.h"
#include "param/synth_param.h"

namespace mana {
class Synth;
}

namespace mana {
class Resynthesis : public IProcessor {
public:
    Resynthesis(Synth& s) : synth_(s) {}

    void Init(float sample_rate) override {}
    void Process(Partials& partials) override;
    void OnUpdateTick(const SynthParam& param, int skip, int module_idx) override;
    void OnNoteOn(int note) override;
    void OnNoteOff() override {}

    void PreGetFreqDiffsInRatio(Partials& partials);

private:
    static float Db2Gain(float db) {
        return std::exp(0.11512925464970228420089957273422f * db);
    }

    Synth& synth_;
    float frame_pos_{};
    float frame_player_pos_{};

    // parameters
    float formant_mix_{};
    float formant_shift_{};
    float freq_scale_{};
    float frame_offset_{};
    float frame_speed_{};
    float gain_mix_{};
    float gain_makeup_{};
};
}