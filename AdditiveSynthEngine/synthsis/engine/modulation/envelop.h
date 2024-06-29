#pragma once

#include "Modulator.h"

#include <string_view>
#include "engine/oscillor_param.h"
#include "engine/poly_param.h"

namespace mana {
class Envelop : public Modulator {
public:
    Envelop(std::string_view id, int idx) : Modulator(id), idx_(idx) {}

    void Init(float sample_rate, float update_rate) override;
    void PrepareParams(ModulableParams& params) override;
    void OnUpdateTick() override;
    void OnNoteOn(int note) override;
    void OnNoteOff() override;
private:
    enum class EnvState {
        kInit = 0,
        kPredelay,
        kAttack,
        kHold,
        kDecay,
        kSustain,
        kRelease
    };

    const int idx_;
    EnvState env_state_{ EnvState::kInit };
    float env_time_{};
    float update_rate_{};

    ModuFloatParameter* predelay_time_{};
    ModuFloatParameter* attack_time_{};
    ModuFloatParameter* hold_time_{};
    ModuFloatParameter* peak_level_{};
    ModuFloatParameter* decay_time_{};
    ModuFloatParameter* sustain_level_{};
    ModuFloatParameter* release_time_{};
    ModuFloatParameter* att_exp_{};
    ModuFloatParameter* dec_exp_{};
    ModuFloatParameter* rel_exp_{};
};
}