#pragma once

#include "engine/modulation/ParamBank.h"
#include "engine/modulation/Parameter.h"
#include "engine/modulation/curve.h"
#include "engine/resynthsis/resynthsis_data.h"

namespace mana {
class SynthParams {
public:
    SynthParams();

    ParamBank& GetParamBank() { return param_bank_; }

    CurveManager& GetCurveManager() { return curve_manager_; }

    void SetResynthsisFrames(ResynthsisFrames new_frame) { resynthsis_frames_ = std::move(new_frame); }
    ResynthsisFrames& GetResynthsisFrames() { return resynthsis_frames_; }
private:
    CurveManager curve_manager_;
    ParamBank param_bank_;
    ResynthsisFrames resynthsis_frames_;
};
}