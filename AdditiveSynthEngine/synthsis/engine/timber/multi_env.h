#pragma once

#include "engine/modulation/curve_v2.h"
#include "timber_frame.h"
#include "osc_param.h"
#include "engine/oscillor_param.h"

namespace mana {
class MultiEnvelop {
public:
    void Init(float sample_rate, float update_rate);
    void PrepareParam(OscillorParams& p);
    void Process(TimberFrame& frame);
    void OnUpdateTick(OscParam& params);
    void OnNoteOn(int note);
    void OnNoteOff();
private:
    CurveV2* pattack_map_{};
    CurveV2* pdecay_map_{};
    CurveV2* ppeak_map_{};
    CurveV2* ppredelay_map_{};
    float attack_time_{};
    float decay_time_{};
    float peak_level_{};
    float predelay_time_{};

    enum class EnvState {
        kInit,
        kPredelay,
        kAttack,
        kDecay
    };
    std::array<float, kNumPartials> env_times_{};
    std::array<EnvState, kNumPartials> env_states_{};
    float update_rate_{};
};
}