#pragma once

#include "engine/oscillor_param.h"
#include "engine/partials.h"

namespace mana {
class FreqProcessor {
public:
    void Init(float sample_rate, float update_rate);
    void OnUpdateTick();
    void OnNoteOn(int note);
    void OnNoteOff();
    void PrepareParams(OscillorParams& params);
    void Process(Partials& partials);
private:
    PolyModuFloatParameter* pitch_bend_;
    float note_pitch_{};
    float base_frequency_{};
    float base_pitch_{};
};
}