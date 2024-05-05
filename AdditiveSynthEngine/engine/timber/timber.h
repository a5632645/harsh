#pragma once

#include "engine/IProcessor.h"
#include "timber_gen.h"

namespace mana {
class Timber : public IProcessor {
public:
    void Init(float sample_rate) override;
    void Process(Partials& partials) override;
    void OnUpdateTick(const SynthParam& params, int skip, int module_idx) override;
    void OnNoteOn(int note) override;
    void OnNoteOff() override;
private:
    TimberFrame osc1_timber_{};
    TimberFrame osc2_timber_{};
    TimberGen osc1_;
    TimberGen osc2_;

    bool is_work_{};
    int osc2_timber_shift_{};
    float osc2_beating_{};
    float osc1_gain_{};
    float osc2_gain_{};
    float beating_phase_{};
    float inv_sample_rate_{};
};
}