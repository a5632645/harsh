#pragma once

#include "engine/IProcessor.h"
#include "param/timber_param.h"
#include "sync.h"
#include "dual_saw.h"
#include "timber_frame.h"

namespace mana {
class TimberGen {
public:
    void Init(float sample_rate);
    void Process(TimberFrame& frame);
    void OnUpdateTick(const OscillorParams& params, int skip, int module_idx);
    void OnNoteOn(int note);
    void OnNoteOff();

    void SetTimberType(param::TimberType::ParamEnum type) { timber_type_ = type; }
private:
    param::TimberType::ParamEnum timber_type_;
    DualSaw dual_saw_;
    Sync sync_;
};
}