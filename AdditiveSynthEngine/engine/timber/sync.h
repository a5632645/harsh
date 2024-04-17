#pragma once

#include "engine/IProcessor.h"
#include "param/synth_param.h"
#include "param/timber.h"

namespace mana {
class Sync : public IProcessor {
public:
    // 通过 IProcessor 继承
    void Init(float sample_rate) override;
    void Process(Partials& partials) override;
    void OnUpdateTick(const SynthParam& params, int skip) override;
    void OnNoteOn(int note) override;
    void OnNoteOff() override;

private:
    param::Sync_WaveShape::WaveShape first_shape_;
    param::Sync_WaveShape::WaveShape second_shape_;
    float fraction_;
    float sync_ratio_;
};
}