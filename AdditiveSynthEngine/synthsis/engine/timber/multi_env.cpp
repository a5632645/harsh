#include "multi_env.h"
#include "param/timber_param.h"
#include "utli/convert.h"

namespace mana {
void MultiEnvelop::Init(float sample_rate, float update_rate) {
    update_rate_ = update_rate;
}

void MultiEnvelop::PrepareParam(OscillorParams& p) {
    auto& cb = p.GetParentSynthParams().GetCurveBank();
    pattack_map_ = cb.GetCurvePtr("timber.env.attack");
    pdecay_map_ = cb.GetCurvePtr("timber.env.decay");
    ppredelay_map_ = cb.GetCurvePtr("timber.env.predelay");
    ppeak_map_ = cb.GetCurvePtr("timber.env.peak");
}

void MultiEnvelop::Process(TimberFrame& frame) {
    constexpr auto min_db = -300.0f;
    for (int i = 0; i < kNumPartials; ++i) {
        auto peak = std::lerp(min_db, peak_level_, ppeak_map_->Get(i));

        switch (env_states_[i]) {
        case EnvState::kInit:
        case EnvState::kPredelay:
            frame.gains[i] = 0.0f;
            break;
        case EnvState::kAttack:
            frame.gains[i] = utli::DbToGain(std::lerp(min_db, peak, env_times_[i]));
            break;
        case EnvState::kDecay:
            frame.gains[i] = utli::DbToGain(std::lerp(peak, min_db, env_times_[i]));
            break;
        default:
            assert(false);
            break;
        }
    }
}

void MultiEnvelop::OnUpdateTick(OscParam& params) {
    attack_time_ = param::MultiEnv_AttackTime::GetNumber(params.args[param::MultiEnv_AttackTime::kArgIdx]->GetValue());
    decay_time_ = param::MultiEnv_DecayTime::GetNumber(params.args[param::MultiEnv_DecayTime::kArgIdx]->GetValue());
    peak_level_ = param::MultiEnv_PeakLevel::GetNumber(params.args[param::MultiEnv_PeakLevel::kArgIdx]->GetValue());
    predelay_time_ = param::MultiEnv_PreDelayTime::GetNumber(params.args[param::MultiEnv_PreDelayTime::kArgIdx]->GetValue());

    for (int i = 0; i < kNumPartials; ++i) {
        switch (env_states_[i]) {
        case EnvState::kAttack:
        {
            auto att_time = attack_time_ * pattack_map_->Get(i);
            auto dec_time = decay_time_ * pdecay_map_->Get(i);
            if (att_time == 0.0f) {
                if (dec_time == 0.0f) {
                    env_states_[i] = EnvState::kInit;
                }
                else {
                    env_states_[i] = EnvState::kDecay;
                    env_times_[i] = 0.0f;
                }
            }
            else {
                auto att_rate = 1.0f / (att_time * update_rate_);
                auto t = env_times_[i] + att_rate;
                if (t > 1.0f) {
                    if (dec_time == 0.0f) {
                        env_states_[i] = EnvState::kInit;
                    }
                    else {
                        env_states_[i] = EnvState::kDecay;
                        t -= 1.0f;
                    }
                }
                env_times_[i] = t;
            }
        }
        break;
        case EnvState::kDecay:
        {
            auto dec_time = decay_time_ * pdecay_map_->Get(i);
            if (dec_time == 0.0f) {
                env_states_[i] = EnvState::kInit;
            }
            else {
                auto dec_rate = 1.0 / (dec_time * update_rate_);
                auto t = env_times_[i] + dec_rate;
                if (t > 1.0f) {
                    env_states_[i] = EnvState::kInit;
                }
                env_times_[i] = t;
            }
        }
        break;
        case EnvState::kPredelay:
        {
            auto pd_time = predelay_time_ * ppredelay_map_->Get(i);
            auto att_time = attack_time_ * pattack_map_->Get(i);
            auto dec_time = decay_time_ * pdecay_map_->Get(i);
            if (pd_time != 0.0f) {
                auto pd_rate = 1.0f / (pd_time * update_rate_);
                auto t = env_times_[i] + pd_rate;
                if (t > 1.0f) {
                    env_times_[i] = 0.0;
                    if (att_time == 0.0f) {
                        if (dec_time == 0.0f) {
                            env_states_[i] = EnvState::kInit;
                        }
                        else {
                            env_states_[i] = EnvState::kDecay;
                        }
                    }
                    else {
                        env_states_[i] = EnvState::kAttack;
                    }
                }
                else {
                    env_times_[i] = t;
                }
            }
            else {
                if (att_time != 0.0f) {
                    env_states_[i] = EnvState::kAttack;
                }
                else {
                    if (dec_time == 0.0f) {
                        env_states_[i] = EnvState::kInit;
                    }
                    else {
                        env_states_[i] = EnvState::kDecay;
                    }
                }
            }
        }
        break;
        default:
            break;
        }
    }
}

void MultiEnvelop::OnNoteOn(int note) {
    for (int i = 0; i < kNumPartials; ++i) {
        auto pre_time = predelay_time_ * ppredelay_map_->Get(i);
        auto att_time = attack_time_ * pattack_map_->Get(i);
        auto dec_time = decay_time_ * pdecay_map_->Get(i);
        env_times_[i] = 0.0f;

        if (pre_time != 0.0f) {
            env_states_[i] = EnvState::kPredelay;
        }
        else {
            if (att_time != 0.0f) {
                env_states_[i] = EnvState::kAttack;
            }
            else {
                if (dec_time == 0.0f) {
                    env_states_[i] = EnvState::kInit;
                }
                else {
                    env_states_[i] = EnvState::kDecay;
                }
            }
        }
    }
}

void MultiEnvelop::OnNoteOff() {
    for (int i = 0; i < kNumPartials; ++i) {
        env_states_[i] = EnvState::kInit;
    }
}
}