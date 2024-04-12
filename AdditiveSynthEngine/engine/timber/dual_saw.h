#pragma once

#include "engine/IProcessor.h"
#include "param/timber_param.h"

namespace mana {

class DualSaw : public IProcessor {
public:
    DualSaw(const param::DualSawParam& param);

    // 通过 IProcessor 继承
    void Init(float sample_rate) override;
    void Process(Partials& partials) override;
    void OnUpdateTick(int skip) override;
    void OnNoteOn(int note) override;
    void OnNoteOff() override;

private:
    const param::DualSawParam& synth_param_;

    float sample_rate_{};
    float beating_phase_{};
};

}