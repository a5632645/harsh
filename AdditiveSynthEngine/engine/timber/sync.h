#pragma once

#include "engine/oscillor_param.h"
#include "param/timber_param.h"
#include "timber_frame.h"

namespace mana {
class Sync {
public:
    void Init(float sample_rate);
    void Process(TimberFrame& frame);
    void OnUpdateTick(const OscillorParams& params, int skip, int module_idx);
    void OnNoteOn(int note);
    void OnNoteOff();

private:
    param::Sync_WaveShape::ParamEnum first_shape_;
    param::Sync_WaveShape::ParamEnum second_shape_;
    float fraction_;
    float sync_ratio_;
};
}