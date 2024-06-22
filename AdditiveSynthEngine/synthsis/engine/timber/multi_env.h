#pragma once

#include "engine/oscillor_param.h"
#include "engine/partials.h"
#include "engine/poly_param.h"

namespace mana {
class MultiEnvelop {
public:
    void Init(float sample_rate, float update_rate);
    void PrepareParam(OscillorParams& p);
    void OnUpdateTick();
    void Process(Partials& frame);
    void OnNoteOn(int note);
    void OnNoteOff();

    bool IsAllMute() const { return std::ranges::all_of(env_states_, [](auto v) { return v == EnvState::kInit; }); }
private:
    PolyModuFloatParameter* predelay_time_{};
    PolyModuFloatParameter* attack_time_{};
    PolyModuFloatParameter* hold_time_{};
    PolyModuFloatParameter* peak_level_{};
    PolyModuFloatParameter* decay_time_{};
    PolyModuFloatParameter* sustain_level_{};
    PolyModuFloatParameter* release_time_{};
    PolyModuFloatParameter* high_scale_{};

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