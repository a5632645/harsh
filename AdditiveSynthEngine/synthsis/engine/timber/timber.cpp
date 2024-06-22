#include "timber.h"

#include <algorithm>
#include "param/timber_param.h"
#include "utli/convert.h"
#include "engine/oscillor_param.h"
#include "timber_gen.h"

namespace mana {
Timber::Timber()
    : osc1_(std::make_unique<TimberGen>(0))
    , osc2_(std::make_unique<TimberGen>(1)) {
}

Timber::~Timber() = default;
Timber::Timber(Timber &&) noexcept = default;
Timber & Timber::operator=(Timber &&) noexcept = default;

void Timber::Init(float sample_rate, float update_rate) {
    osc1_->Init(sample_rate, update_rate);
    osc2_->Init(sample_rate, update_rate);
    inv_update_rate_ = 1.0f / update_rate;
}

void Timber::Process(Partials& partials) {
    osc1_->Process(osc1_timber_);
    std::ranges::transform(osc1_timber_.gains, osc1_timber_.gains.begin(),
                           [this](float v) {return v * osc1_gain_; });
    std::ranges::copy(osc1_timber_.gains, partials.gains.begin());

    if (osc2_gain_ != 0.0f) {
        osc2_->Process(osc2_timber_);
        std::ranges::transform(osc2_timber_.gains, osc2_timber_.gains.begin(),
                               [this](float v) {return v * osc2_gain_; });
        int proc_idx = 0;
        for (int i = osc2_timber_shift_ - 1; i < kNumPartials; i += osc2_timber_shift_) {
            auto fade = std::cos(std::numbers::pi_v<float> *2.0f * partials.ratios[i] * beating_phase_);
            auto fade01 = 0.5f + 0.5f * fade;
            partials.gains[i] = std::lerp(osc1_timber_.gains[i] - osc2_timber_.gains[proc_idx],
                                          osc1_timber_.gains[i] + osc2_timber_.gains[proc_idx],
                                          fade01);
            ++proc_idx;
        }
    }
}

void Timber::PrepareParams(OscillorParams & params) {
    arg_osc2_timber_shift_ = params.GetPolyFloatParam("timber.osc2_shift");
    arg_osc2_beating_ = params.GetPolyFloatParam("timber.osc2_beating");
    arg_osc1_gain_ = params.GetPolyFloatParam("timber.osc1_gain");
    arg_osc2_gain_ = params.GetPolyFloatParam("timber.osc2_gain");
    osc1_->PrepareParams(params);
    osc2_->PrepareParams(params);
}

void Timber::OnUpdateTick() {
    osc1_->OnUpdateTick();
    osc2_->OnUpdateTick();

    osc2_timber_shift_ = arg_osc2_timber_shift_->GetInt();
    osc2_beating_ = arg_osc2_beating_->GetValue();
    auto inc = osc2_beating_ * inv_update_rate_;
    beating_phase_ += inc;
    beating_phase_ -= static_cast<int>(beating_phase_);

    osc1_gain_ = utli::DbToGain(arg_osc1_gain_->GetValue(),
                                param::Timber_OscGain::kMin);
    osc2_gain_ = utli::DbToGain(arg_osc2_gain_->GetValue(),
                                param::Timber_OscGain::kMin);
}

void Timber::OnNoteOn(int note) {
    osc1_->OnNoteOn(note);
    osc2_->OnNoteOn(note);

    if (osc2_beating_ == 0.0f) {
        beating_phase_ = 0.0f;
    }
}

void Timber::OnNoteOff() {
    osc1_->OnNoteOff();
    osc2_->OnNoteOff();
}
}