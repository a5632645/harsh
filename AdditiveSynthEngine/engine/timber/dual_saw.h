#pragma once

#include "timber_frame.h"
#include "osc_param.h"

namespace mana {
class DualSaw {
public:
    void Init(float sample_rate, float update_rate);
    void Process(TimberFrame& frame);
    void OnUpdateTick(OscParam& params);
    void OnNoteOn(int note);
    void OnNoteOff();
private:
    float ratio_{};
    float beating_rate_{};
    float saw_square_{};
    float sample_rate_{};
    float inv_update_rate_{};
    float beating_phase_{};
    float second_amp_{};
};
}