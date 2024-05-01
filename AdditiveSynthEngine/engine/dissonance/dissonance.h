#pragma once

#include "engine/IProcessor.h"
#include "param/dissonance_param.h"

namespace mana {
class Dissonance : public IProcessor {
public:
    void Init(float sample_rate) override;
    void Process(Partials & partials) override;
    void OnUpdateTick(const SynthParam& params, int skip, int module_idx) override;
    void OnNoteOn(int note) override;
    void OnNoteOff() override;
private:
    bool is_enable_;
    param::DissonanceType::ParamEnum type_;

    // string
    float string_stretch_factor_;

    // harm stretch
    float harmonic_stretch_ratio_;

    // st space
    float st_space_semitone_;
};
}