#pragma once

#include "timber_param.h"
#include "voice_param.h"
#include "timber_proc_param.h"

namespace mana::param {

struct SynthParam {
    TimberParam timber_param;
    TimberProcParam timber_proc_param;
    VoiceParam voice_param;
};

}