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
    void PrepareParams(OscillorParams& params) override;
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

    PolyModuFloatParameter* predelay_time_{};
    PolyModuFloatParameter* attack_time_{};
    PolyModuFloatParameter* hold_time_{};
    PolyModuFloatParameter* peak_level_{};
    PolyModuFloatParameter* decay_time_{};
    PolyModuFloatParameter* sustain_level_{};
    PolyModuFloatParameter* release_time_{};
    PolyModuFloatParameter* att_exp_{};
    PolyModuFloatParameter* dec_exp_{};
    PolyModuFloatParameter* rel_exp_{};
};
}