#include "envelop.h"

#include "param/envelop_param.h"
#include "utli/warp.h"

namespace mana {
void Envelop::Init(float sample_rate, float update_rate) {
    update_rate_ = update_rate;
}

void Envelop::PrepareParams(OscillorParams& p) {
    predelay_time_ = p.GetPolyFloatParam("envelop{}.predelay", idx_);
    attack_time_ = p.GetPolyFloatParam("envelop{}.attack", idx_);
    hold_time_ = p.GetPolyFloatParam("envelop{}.hold", idx_);
    decay_time_ = p.GetPolyFloatParam("envelop{}.decay", idx_);
    peak_level_ = p.GetPolyFloatParam("envelop{}.peak", idx_);
    sustain_level_ = p.GetPolyFloatParam("envelop{}.sustain", idx_);
    release_time_ = p.GetPolyFloatParam("envelop{}.release", idx_);
    att_exp_ = p.GetPolyFloatParam("envelop{}.att_exp", idx_);
    dec_exp_ = p.GetPolyFloatParam("envelop{}.dec_exp", idx_);
    rel_exp_ = p.GetPolyFloatParam("envelop{}.rel_exp", idx_);
}

void Envelop::OnUpdateTick() {
    auto predelay = predelay_time_->GetValue();
    auto attack = attack_time_->GetValue();
    auto hold = hold_time_->GetValue();
    auto decay = decay_time_->GetValue();
    auto peak = peak_level_->GetValue();
    auto sustain = sustain_level_->GetValue();
    auto release = release_time_->GetValue();

    switch (env_state_) {
    case EnvState::kInit:
    case EnvState::kSustain:
        break;
    case EnvState::kPredelay:
    {
        if (predelay != 0.0f) {
            auto rate = 1.0f / (predelay * update_rate_);
            env_time_ += rate;
            if (env_time_ <= 1.0f) {
                break;
            }
        }
        if (attack != 0.0f) {
            env_state_ = EnvState::kAttack;
            env_time_ = 0.0f;
            break;
        }
        if (hold != 0.0f) {
            env_state_ = EnvState::kHold;
            env_time_ = 0.0f;
            break;
        }
        if (decay != 0.0f) {
            env_state_ = EnvState::kDecay;
            env_time_ = 0.0f;
            break;
        }
        env_state_ = EnvState::kSustain;
    }
    break;
    case EnvState::kAttack:
    {
        if (attack != 0.0f) {
            auto rate = 1.0f / (attack * update_rate_);
            env_time_ += rate;
            if (env_time_ <= 1.0f) {
                break;
            }
        }
        if (hold != 0.0f) {
            env_state_ = EnvState::kHold;
            env_time_ = 0.0f;
            break;
        }
        if (decay != 0.0f) {
            env_state_ = EnvState::kDecay;
            env_time_ = 0.0f;
            break;
        }
        env_state_ = EnvState::kSustain;
    }
    break;
    case EnvState::kHold:
    {
        if (hold != 0.0f) {
            auto rate = 1.0f / (hold * update_rate_);
            env_time_ += rate;
            if (env_time_ <= 1.0f) {
                break;
            }
        }
        if (decay != 0.0f) {
            env_state_ = EnvState::kDecay;
            env_time_ = 0.0f;
            break;
        }
        env_state_ = EnvState::kSustain;
    }
    break;
    case EnvState::kDecay:
    {
        if (decay != 0.0f) {
            auto rate = 1.0f / (decay * update_rate_);
            env_time_ += rate;
            if (env_time_ <= 1.0f) {
                break;
            }
        }
        env_state_ = EnvState::kSustain;
    }
    break;
    case EnvState::kRelease:
    {
        if (release != 0.0f) {
            auto rate = 1.0f / (release * update_rate_);
            env_time_ += rate;
            if (env_time_ <= 1.0f) {
                break;
            }
        }
        env_state_ = EnvState::kInit;
    }
    break;
    default:
        break;
    }

    switch (env_state_) {
    case mana::Envelop::EnvState::kInit:
    case mana::Envelop::EnvState::kPredelay:
        SetOutput(0.0f);
        break;
    case mana::Envelop::EnvState::kAttack:
        SetOutput(peak * utli::warp::ExpWarp(env_time_, att_exp_->GetValue()));;
        break;
    case mana::Envelop::EnvState::kHold:
        SetOutput(peak);
        break;
    case mana::Envelop::EnvState::kDecay:
    {
        auto x = utli::warp::ExpWarp(env_time_, dec_exp_->GetValue());
        SetOutput(std::lerp(peak, sustain, x));
    }
        break;
    case mana::Envelop::EnvState::kSustain:
        SetOutput(sustain);
        break;
    case mana::Envelop::EnvState::kRelease:
        SetOutput((1.0f - utli::warp::ExpWarp(env_time_, rel_exp_->GetValue())) * sustain);
        break;
    default:
        assert(false);
        break;
    }
}

void Envelop::OnNoteOn(int note) {
    auto predelay = predelay_time_->GetValue();
    auto attack = attack_time_->GetValue();
    auto hold = hold_time_->GetValue();
    auto decay = decay_time_->GetValue();

    env_time_ = 0.0f;
    if (predelay != 0.0f) {
        env_state_ = EnvState::kPredelay;
        return;
    }
    if (attack != 0.0f) {
        env_state_ = EnvState::kAttack;
        return;
    }
    if (hold != 0.0f) {
        env_state_ = EnvState::kHold;
        return;
    }
    if (decay != 0.0f) {
        env_state_ = EnvState::kDecay;
        return;
    }
    env_state_ = EnvState::kSustain;
}

void Envelop::OnNoteOff() {
    auto release = release_time_->GetValue();

    if (release != 0.0f) {
        env_state_ = EnvState::kRelease;
        env_time_ = 0.0f;
        return;
    }
    env_state_ = EnvState::kInit;
}
}