#pragma once

#include "param/synth_param.h"
#include "timber_frame.h"

namespace mana {
class DualSaw {
public:
    void Init(float sample_rate);
    void Process(TimberFrame& frame);
    void OnUpdateTick(const SynthParam& param, int skip, int module_idx);
    void OnNoteOn(int note);
    void OnNoteOff();
private:
    float ratio_{};
    float beating_rate_{};
    float saw_square_{};
    float sample_rate_{};
    float beating_phase_{};
    float second_amp_{};
};
}