#pragma once

#include "engine/IProcessor.h"
#include "param/synth_param.h"
#include "param/timber_param.h"

namespace mana {
class Sync : public IProcessor {
public:
    // 通过 IProcessor 继承
    void Init(float sample_rate) override;
    void Process(Partials& partials) override;
    void OnUpdateTick(const SynthParam& params, int skip, int module_idx) override;
    void OnNoteOn(int note) override;
    void OnNoteOff() override;

private:
    param::Sync_WaveShape::ParamEnum first_shape_;
    param::Sync_WaveShape::ParamEnum second_shape_;
    float fraction_;
    float sync_ratio_;
};
}