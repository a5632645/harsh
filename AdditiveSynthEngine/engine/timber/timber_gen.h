#pragma once

#include "param/timber_param.h"
#include "sync.h"
#include "dual_saw.h"
#include "timber_frame.h"
#include "osc_param.h"
#include "engine/oscillor_param.h"

namespace mana {
class TimberGen {
public:
    TimberGen(int idx);

    void Init(float sample_rate, float update_rate);
    void Process(TimberFrame& frame);
    void PrepareParams(OscillorParams& params);
    void OnUpdateTick();
    void OnNoteOn(int note);
    void OnNoteOff();

    void SetTimberType(param::TimberType::ParamEnum type) { timber_type_ = type; }
private:
    const int idx_;
    OscParam osc_param_;
    IntParameter* timber_type_arg_;
    param::TimberType::ParamEnum timber_type_;
    DualSaw dual_saw_;
    Sync sync_;
};
}