#include "envelop.h"

#include "param/envelop_param.h"

namespace mana {
void Envelop::Init(float sample_rate, float update_rate) {
    update_time_interval_ = 1000.0f / update_rate;
}

void Envelop::PrepareParams(OscillorParams& params) {
    arg_attack_ms_ = params.GetPolyFloatParam("envelop{}.attack", idx_);
    arg_decay_ms_ = params.GetPolyFloatParam("envelop{}.decay", idx_);
    arg_sustain_level_ = params.GetPolyFloatParam("envelop{}.sustain", idx_);
    arg_release_ms_ = params.GetPolyFloatParam("envelop{}.release", idx_);
}

void Envelop::OnUpdateTick() {
    auto att_t = param::Env_Attack::GetNumber(arg_attack_ms_->GetValue());
    auto dec_t = param::Env_Decay::GetNumber(arg_decay_ms_->GetValue());
    auto sus_v = param::Env_Sustain::GetNumber(arg_sustain_level_->GetValue());
    auto rel_t = param::Env_Release::GetNumber(arg_release_ms_->GetValue());

    if (env_state_ == State::kInit) {
        output_value_ = 0.0f;
        return;
    }

    if (env_state_ == State::kAttack) {
        if (att_t == 0.0f || state_offset_ms_ >= att_t) {
            env_state_ = State::kDecay;
            state_offset_ms_ = 0.0f;
        }
        else {
            output_value_ = state_offset_ms_ / att_t;
            state_offset_ms_ += update_time_interval_;
            return;
        }
    }

    if (env_state_ == State::kDecay) {
        if (dec_t == 0.0f || state_offset_ms_ >= dec_t) {
            env_state_ = State::kSustain;
            state_offset_ms_ = 0.0f;
        }
        else {
            output_value_ = std::lerp(1.0f, sus_v, state_offset_ms_ / dec_t);
            state_offset_ms_ += update_time_interval_;
            return;
        }
    }

    if (env_state_ == State::kSustain) {
        output_value_ = sus_v;
        return;
    }

    if (env_state_ == State::kRelease) {
        if (rel_t == 0.0f || state_offset_ms_ >= rel_t) {
            output_value_ = 0.0f;
            state_offset_ms_ = 0.0f;
            env_state_ = State::kInit;
        }
        else {
            output_value_ = sus_v * (1.0f - state_offset_ms_ / rel_t);
            state_offset_ms_ += update_time_interval_;
        }
    }
}

void Envelop::OnNoteOn(int note) {
    state_offset_ms_ = 0.0f;

    env_state_ = State::kAttack;
    if (arg_attack_ms_->GetValue() != 0.0f) {
        return;
    }

    env_state_ = State::kDecay;
    if (arg_decay_ms_->GetValue() != 0.0f) {
        return;
    }

    env_state_ = State::kSustain;
}

void Envelop::OnNoteOff() {
    if (env_state_ != State::kInit) {
        env_state_ = State::kRelease;
    }
    state_offset_ms_ = 0.0f;
}
}