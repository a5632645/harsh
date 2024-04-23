#pragma once

#include "engine/IProcessor.h"
#include "param/dissonance_param.h"
#include "string.h"

namespace mana {
class Dissonance : public IProcessor {
public:
    // 通过 IProcessor 继承
    void Init(float sample_rate) override;
    void Process(Partials & partials) override;
    void OnUpdateTick(const SynthParam& params, int skip, int module_idx) override;
    void OnNoteOn(int note) override;
    void OnNoteOff() override;
private:
    bool is_enable_;
    param::DissonanceType::DissonaceTypeEnum type_;
    StringDissonance decay_;
    float harmonic_stretch_ratio_;
    float st_space_semitone_;
};
}