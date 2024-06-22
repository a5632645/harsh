#include "multi_env.h"
#include "param/timber_param.h"
#include "utli/convert.h"

namespace mana {
void MultiEnvelop::Init(float sample_rate, float update_rate) {
    update_rate_ = update_rate;
}

void MultiEnvelop::PrepareParam(OscillorParams& p) {
    predelay_time_ = p.GetPolyFloatParam("vol_env.predelay");
    attack_time_ = p.GetPolyFloatParam("vol_env.attack");
    hold_time_ = p.GetPolyFloatParam("vol_env.hold");
    decay_time_ = p.GetPolyFloatParam("vol_env.decay");
    peak_level_ = p.GetPolyFloatParam("vol_env.peak");
    sustain_level_ = p.GetPolyFloatParam("vol_env.sustain");
    release_time_ = p.GetPolyFloatParam("vol_env.release");
    high_scale_ = p.GetPolyFloatParam("vol_env.high_scale");
}

void MultiEnvelop::Process(Partials& frame) {
    constexpr auto min_db = -60.0f;
    auto high_scale = high_scale_->GetValue();
    auto predelay = predelay_time_->GetValue();
    auto attack = attack_time_->GetValue();
    auto hold = hold_time_->GetValue();
    auto decay = decay_time_->GetValue();
    auto peak = peak_level_->GetValue();
    auto sustain = sustain_level_->GetValue();
    auto release = release_time_->GetValue();
    auto sustain_gain = utli::DbToGain(sustain);
    auto peak_gain = utli::DbToGain(peak);

    for (int i = 0; i < kNumPartials; ++i) {
        constexpr auto inv_max_ratio = 1.0f / kNumPartials;
        auto lerp = std::clamp(frame.ratios[i] * inv_max_ratio, 0.0f, 5.0f);
        auto time_scale = std::max(0.0f, 1.0f + (high_scale - 1.0f) * lerp);
        auto pre_time = predelay * time_scale;
        auto att_time = attack * time_scale;
        auto hold_time = hold * time_scale;
        auto dec_time = decay * time_scale;
        auto rel_time = release * time_scale;

        switch (env_states_[i]) {
        case EnvState::kInit:
        case EnvState::kSustain:
            break;
        case EnvState::kPredelay:
        {
            if (pre_time != 0.0f) {
                auto rate = 1.0f / (pre_time * update_rate_);
                env_times_[i] += rate;
                if (env_times_[i] <= 1.0f) {
                    break;
                }
            }
            if (att_time != 0.0f) {
                env_states_[i] = EnvState::kAttack;
                env_times_[i] = 0.0f;
                break;
            }
            if (hold_time != 0.0f) {
                env_states_[i] = EnvState::kHold;
                env_times_[i] = 0.0f;
                break;
            }
            if (dec_time != 0.0f) {
                env_states_[i] = EnvState::kDecay;
                env_times_[i] = 0.0f;
                break;
            }
            env_states_[i] = EnvState::kSustain;
        }
        break;
        case EnvState::kAttack:
        {
            if (att_time != 0.0f) {
                auto rate = 1.0f / (att_time * update_rate_);
                env_times_[i] += rate;
                if (env_times_[i] <= 1.0f) {
                    break;
                }
            }
            if (hold_time != 0.0f) {
                env_states_[i] = EnvState::kHold;
                env_times_[i] = 0.0f;
                break;
            }
            if (dec_time != 0.0f) {
                env_states_[i] = EnvState::kDecay;
                env_times_[i] = 0.0f;
                break;
            }
            env_states_[i] = EnvState::kSustain;
        }
        break;
        case EnvState::kHold:
        {
            if (hold_time != 0.0f) {
                auto rate = 1.0f / (hold_time * update_rate_);
                env_times_[i] += rate;
                if (env_times_[i] <= 1.0f) {
                    break;
                }
            }
            if (dec_time != 0.0f) {
                env_states_[i] = EnvState::kDecay;
                env_times_[i] = 0.0f;
                break;
            }
            env_states_[i] = EnvState::kSustain;
        }
        break;
        case EnvState::kDecay:
        {
            if (dec_time != 0.0f) {
                auto rate = 1.0f / (dec_time * update_rate_);
                env_times_[i] += rate;
                if (env_times_[i] <= 1.0f) {
                    break;
                }
            }
            env_states_[i] = EnvState::kSustain;
        }
        break;
        case EnvState::kRelease:
        {
            if (rel_time != 0.0f) {
                auto rate = 1.0f / (rel_time * update_rate_);
                env_times_[i] += rate;
                if (env_times_[i] <= 1.0f) {
                    break;
                }
            }
            env_states_[i] = EnvState::kInit;
        }
        break;
        default:
            break;
        }
    }

    for (int i = 0; i < kNumPartials; ++i) {
        switch (env_states_[i]) {
        case EnvState::kInit:
        case EnvState::kPredelay:
            frame.gains[i] = 0.0f;
            break;
        case EnvState::kAttack:
            frame.gains[i] *= utli::DbToGain(std::lerp(min_db, peak, env_times_[i]));
            break;
        case EnvState::kDecay:
            frame.gains[i] *= utli::DbToGain(std::lerp(peak, min_db, env_times_[i]));
            break;
        case EnvState::kHold:
            frame.gains[i] *= peak_gain;
            break;
        case EnvState::kSustain:
            frame.gains[i] *= sustain_gain;
            break;
        case EnvState::kRelease:
            frame.gains[i] *= utli::DbToGain(std::lerp(sustain, min_db, env_times_[i]));
            break;
        default:
            assert(false);
            break;
        }
    }
}

void MultiEnvelop::OnUpdateTick() {
}

void MultiEnvelop::OnNoteOn(int note) {
    auto predelay = predelay_time_->GetValue();
    auto attack = attack_time_->GetValue();
    auto hold = hold_time_->GetValue();
    auto decay = decay_time_->GetValue();

    std::ranges::fill(env_times_, 0.0f);
    if (predelay != 0.0f) {
        env_states_.fill(EnvState::kPredelay);
        return;
    }
    if (attack != 0.0f) {
        env_states_.fill(EnvState::kAttack);
        return;
    }
    if (hold != 0.0f) {
        env_states_.fill(EnvState::kHold);
        return;
    }
    if (decay != 0.0f) {
        env_states_.fill(EnvState::kDecay);
        return;
    }
    env_states_.fill(EnvState::kSustain);
}

void MultiEnvelop::OnNoteOff() {
    auto release = release_time_->GetValue();

    if (release != 0.0f) {
        env_states_.fill(EnvState::kRelease);
        env_times_.fill(0.0f);
        return;
    }
    env_states_.fill(EnvState::kInit);
}
}