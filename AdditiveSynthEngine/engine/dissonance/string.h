#pragma once

#include "engine/IProcessor.h"
#include "param/dissonance_param.h"

namespace mana {
class StringDissonance : public IProcessor {
public:
    // 通过 IProcessor 继承
    void Init(float sample_rate) override;
    void Process(Partials& partials) override;
    void OnUpdateTick(const SynthParam& param, int skip) override;
    void OnNoteOn(int note) override;
    void OnNoteOff() override;
private:
    float stretch_factor_;
};
}