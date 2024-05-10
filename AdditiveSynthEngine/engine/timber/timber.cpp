#include "timber.h"

#include <algorithm>
#include "param/timber_param.h"
#include "utli/convert.h"

namespace mana {
void Timber::Init(float sample_rate, float update_rate) {
    osc1_.Init(sample_rate);
    osc2_.Init(sample_rate);
    inv_sample_rate_ = 1.0f / sample_rate;
}

void Timber::Process(Partials& partials) {
    if (!is_work_) {
        partials.gains.fill(float{});
        return;
    }

    osc1_.Process(osc1_timber_);
    std::ranges::transform(osc1_timber_.gains, osc1_timber_.gains.begin(),
                           [this](float v) {return v * osc1_gain_; });
    std::ranges::copy(osc1_timber_.gains, partials.gains.begin());

    //auto phase_magic_val = std::numbers::pi_v<float> *beating_phase_;
    /*for (int i = 0; i < kNumPartials; ++i) {
        auto vector_angle = (i + 1.0f) * phase_magic_val;
        auto power_add = osc1_timber_.gains[i] * osc1_timber_.gains[i]
            + osc2_timber_.gains[i] * osc2_timber_.gains[i]
            + 2.0f * osc1_timber_.gains[i] * osc2_timber_.gains[i] * std::cos(vector_angle);
        partials.gains[i] = std::sqrt(power_add);
    }*/

    if (osc2_gain_ != 0.0f) {
        osc2_.Process(osc2_timber_);
        std::ranges::transform(osc2_timber_.gains, osc2_timber_.gains.begin(),
                               [this](float v) {return v * osc2_gain_; });
        int proc_idx = 0;
        for (int i = osc2_timber_shift_ - 1; i < kNumPartials; i += osc2_timber_shift_) {
            auto fade = std::cos(std::numbers::pi_v<float> *2.0f * (proc_idx + 1.0f) * beating_phase_);
            auto fade01 = 0.5f + 0.5f * fade;
            partials.gains[i] = std::lerp(osc1_timber_.gains[i] - osc2_timber_.gains[proc_idx],
                                          osc1_timber_.gains[i] + osc2_timber_.gains[proc_idx],
                                          fade01);
            ++proc_idx;
        }
    }
}

void Timber::OnUpdateTick(const OscillorParams & params, int skip, int module_idx) {
    osc1_.OnUpdateTick(params, skip, 0);
    osc2_.OnUpdateTick(params, skip, 1);

    osc2_timber_shift_ = static_cast<int>(param::Timber_Osc2Shift::GetNumber(params.timber.osc2_shift));
    osc2_beating_ = param::Timber_Osc2Beating::GetNumber(params.timber.osc2_beating);
    auto inc = osc2_beating_ * inv_sample_rate_ * skip;
    beating_phase_ += inc;
    beating_phase_ -= static_cast<int>(beating_phase_);

    osc1_gain_ = utli::DbToGain(param::Timber_OscGain::GetNumber(params.timber.osc1_gain),
                                param::Timber_OscGain::kMin);
    osc2_gain_ = utli::DbToGain(param::Timber_OscGain::GetNumber(params.timber.osc2_gain),
                                param::Timber_OscGain::kMin);
}

void Timber::OnNoteOn(int note) {
    osc1_.OnNoteOn(note);
    osc2_.OnNoteOn(note);
    is_work_ = true;

    if (osc2_beating_ == 0.0f) {
        beating_phase_ = 0.0f;
    }
}

void Timber::OnNoteOff() {
    osc1_.OnNoteOff();
    osc2_.OnNoteOff();
    is_work_ = false;
}
}