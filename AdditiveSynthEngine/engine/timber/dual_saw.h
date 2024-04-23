#pragma once

#include "engine/IProcessor.h"
#include "param/synth_param.h"

namespace mana {
class DualSaw : public IProcessor {
public:
    // 通过 IProcessor 继承
    void Init(float sample_rate) override;
    void Process(Partials& partials) override;
    void OnUpdateTick(const SynthParam& param, int skip, int module_idx) override;
    void OnNoteOn(int note) override;
    void OnNoteOff() override;

private:
    float ratio_{};
    float beating_rate_{};
    float saw_square_{};
    float sample_rate_{};
    float beating_phase_{};
};
}