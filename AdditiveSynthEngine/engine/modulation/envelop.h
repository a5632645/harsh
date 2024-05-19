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
    enum class State {
        kInit,
        kAttack,
        kDecay,
        kSustain,
        kRelease
    };

    const int idx_;

    PolyModuFloatParameter* arg_attack_ms_{};
    PolyModuFloatParameter* arg_decay_ms_{};
    PolyModuFloatParameter* arg_sustain_level_{};
    PolyModuFloatParameter* arg_release_ms_{};

    State env_state_{ State::kInit };
    float state_offset_ms_{};
    float update_time_interval_{};
};
}