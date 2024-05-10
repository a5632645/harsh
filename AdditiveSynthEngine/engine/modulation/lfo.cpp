#include "lfo.h"

#include <iostream>
#include "param/lfo_param.h"
#include "engine/synth.h"
#include "engine/VolumeTable.hpp"
#include <gcem.hpp>
#include "engine/EngineConfig.h"
#include <numbers>
#include "utli/warp.h"

namespace mana {
static constexpr auto kSineTable = MakeNormalizeTable<kNumPartials>(
    [](float nor) {
    auto raw_sin = gcem::cos(std::numbers::pi_v<float> *2.0f * nor);
    return 0.5f * raw_sin + 0.5f;
}
);
}

namespace mana {
void LFO::Init(float sample_rate, float update_rate) {
    inv_update_rate_ = 1.0f / update_rate;
}

void LFO::OnUpdateTick(const OscillorParams& params, int /*skip*/, int /*module_idx*/) {
    auto lfo_rate = param::LFO_Rate::GetNumber(params.lfos[idx_].rate);
    start_phase_ = param::LFO_Phase::GetNumber(params.lfos[idx_].start_phase);
    restart_ = params.lfos[idx_].restart->GetBool();
    output_level_ = param::LFO_Level::GetNumber(params.lfos[idx_].level);
    wave_type_ = param::LFO_WaveType::GetEnum(params.lfos[idx_].wave_type->GetInt());
    wave_curve_idx_ = params.lfos[idx_].wave_curve_idx->GetInt();
    wave_curve_ = params.parent_synth->GetCurveManager().GetCurvePtr(wave_curve_idx_);

    auto phase_inc = lfo_rate * inv_update_rate_;
    lfo_phase_ += phase_inc;

    if (lfo_phase_ >= 1.0f) {
        lfo_phase_ -= static_cast<int>(lfo_phase_);
        last_random_value_ = urd_(random_);
    }

    using enum param::LFO_WaveType::ParamEnum;
    switch (wave_type_) {
    case kSine:
        output_value_ = utli::warp::AtNormalizeIndex(kSineTable, lfo_phase_);
        break;
    case kTri:
        output_value_ = std::abs(2.0f * lfo_phase_ - 1.0f);
        break;
    case kSawUp:
        output_value_ = lfo_phase_;
        break;
    case kSawDown:
        output_value_ = 1.0f - lfo_phase_;
        break;
    case kSquare:
        output_value_ = lfo_phase_ > 0.5f ? 1.0f : 0.0f;
        break;
    case kRandom:
        output_value_ = urd_(random_);
        break;
    case kSampleAndHold:
        output_value_ = last_random_value_;
        break;
    case kCustom:
        output_value_ = utli::warp::AtNormalizeIndex(wave_curve_->data, lfo_phase_);
        break;
    case kNumEnums:
    default:
        assert(false);
        break;
    }
    output_value_ *= output_level_;
}

void LFO::OnNoteOn(int note) {
    if (restart_) {
        lfo_phase_ = start_phase_;
    }
}
}