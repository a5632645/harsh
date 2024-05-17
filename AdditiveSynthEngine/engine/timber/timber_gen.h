#pragma once

#include "engine/forward_decalre.h"
#include "sync.h"
#include "dual_saw.h"
#include "timber_frame.h"
#include "osc_param.h"
#include "noise.h"
#include "my_pwm.h"
#include "impluse.h"
#include <tuple>

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
private:
    const int idx_;
    OscParam osc_param_{};
    IntParameter* timber_type_arg_{};
    std::tuple<DualSaw,
        Sync,
        Noise,
        PWM,
        Impulse> timber_gens_;
};
}