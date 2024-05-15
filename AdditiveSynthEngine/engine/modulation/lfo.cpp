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

void LFO::PrepareParams(OscillorParams& params) {
    arg_start_phase_ = params.GetPolyFloatParam("lfo{}.start_phase", idx_);
    arg_lfo_rate_ = params.GetPolyFloatParam("lfo{}.rate", idx_);
    restart_ = params.GetParam<BoolParameter>("lfo{}.restart", idx_);
    arg_output_level_ = params.GetPolyFloatParam("lfo{}.level", idx_);
    arg_wave_type_ = params.GetParam<IntParameter>("lfo{}.wave_type", idx_);
    wave_curve_ = params.GetParentSynthParams().GetCurveManager().GetCurvePtr("lfo{}.wave", idx_);
}

void LFO::OnUpdateTick() {
    auto lfo_rate = param::LFO_Rate::GetNumber(arg_lfo_rate_->GetClamp());
    start_phase_ = param::LFO_Phase::GetNumber(arg_start_phase_->GetClamp());
    auto output_level = param::LFO_Level::GetNumber(arg_output_level_->GetClamp());
    wave_type_ = param::LFO_WaveType::GetEnum(arg_wave_type_->GetInt());

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
    output_value_ *= output_level;
}

void LFO::OnNoteOn(int note) {
    if (restart_->GetBool()) {
        lfo_phase_ = start_phase_;
    }
}
}