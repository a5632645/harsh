#pragma once

#include "param/timber_param.h"
#include "timber_frame.h"
#include "osc_param.h"

namespace mana {
class Sync {
public:
    void Init(float sample_rate, float update_rate);
    void Process(TimberFrame& frame);
    void OnUpdateTick(OscParam& params);
    void OnNoteOn(int note);
    void OnNoteOff();

private:
    param::Sync_WaveShape::ParamEnum first_shape_;
    param::Sync_WaveShape::ParamEnum second_shape_;
    float fraction_;
    float sync_ratio_;
};
}