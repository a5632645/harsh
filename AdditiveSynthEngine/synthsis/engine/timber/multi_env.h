#pragma once

#include "engine/oscillor_param.h"
#include "engine/partials.h"
#include "engine/poly_param.h"

namespace mana {
class MultiEnvelop {
public:
    void Init(float sample_rate, float update_rate);
    void PrepareParam(ModulableParams& p);
    void OnUpdateTick();
    void Process(Partials& frame);
    void OnNoteOn(int note);
    void OnNoteOff();

    bool IsAllMute() const { return std::ranges::all_of(env_states_, [](auto v) { return v == EnvState::kInit; }); }
private:
    ModuFloatParameter* predelay_time_{};
    ModuFloatParameter* attack_time_{};
    ModuFloatParameter* hold_time_{};
    ModuFloatParameter* peak_level_{};
    ModuFloatParameter* decay_time_{};
    ModuFloatParameter* sustain_level_{};
    ModuFloatParameter* release_time_{};
    ModuFloatParameter* high_scale_{};

    enum class EnvState {
        kInit = 0,
        kPredelay,
        kAttack,
        kHold,
        kDecay,
        kSustain,
        kRelease
    };
    std::array<float, kNumPartials> env_times_{};
    std::array<EnvState, kNumPartials> env_states_{};
    float update_rate_{};
};
}