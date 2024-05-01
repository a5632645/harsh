#pragma once

#include "engine/IProcessor.h"
#include "param/synth_param.h"

namespace mana {
class Synth;
}

namespace mana {
class Resynthesis : public IProcessor {
public:
    Resynthesis(Synth& synth) : synth_(synth) {}

    void Init(float sample_rate) override {}
    void Process(Partials& partials) override;
    void OnUpdateTick(const SynthParam& param, int skip, int module_idx) override;
    void OnNoteOn(int note) override {}
    void OnNoteOff() override {}

private:
    Synth& synth_;

    float formant_mix_{};
    float formant_shift_{};
    float freq_scale_{};
    float frame_pos_{};
};
}