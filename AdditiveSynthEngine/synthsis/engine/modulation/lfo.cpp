#include "lfo.h"

#include <iostream>
#include "param/lfo_param.h"
#include "engine/synth.h"
#include "engine/VolumeTable.hpp"
#include <gcem.hpp>
#include "engine/EngineConfig.h"
#include <numbers>
#include "utli/warp.h"
#include "param/lfo_param.h"

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
    lfo_mode_ = params.GetParam<IntChoiceParameter>(param::LFO_Mode::kIdFormater, idx_);
    lfo_rate_mode_ = params.GetParam<IntChoiceParameter>(param::LFO_TimeType::kIdFormater, idx_);
    wave_type_ = params.GetParam<IntChoiceParameter>(param::LFO_WaveType::kIdFormater, idx_);
    bpm_ = params.GetParam<FloatParameter>("bpm");
    lfo_rate_ = params.GetPolyFloatParam("lfo{}.rate", idx_);
    start_phase_ = params.GetPolyFloatParam(param::LFO_Phase::kIdFormater, idx_);
    wave_curve_ = params.GetParentSynthParams().GetCurveBank().GetCurvePtr("lfo{}.wave", idx_);
}

void LFO::OnUpdateTick() {
    auto time_type = param::LFO_TimeType::GetEnum(lfo_rate_mode_->GetInt());
    auto raw_rate = lfo_rate_->GetValue();
    auto rate = 0.0f;
    using tt = param::LFO_TimeType::ParamEnum;
    switch (time_type) {
    case tt::kHz:
        rate = param::LFO_HzRate::GetNumber(raw_rate);
        break;
    case tt::kSeconds:
        rate = 1.0f / param::LFO_SecondRate::GetNumber(raw_rate);
        break;
    case tt::kBeats:
    {
        auto beat_rate = bpm_->GetValue() / (60.0f * 4.0f);
        constexpr std::array kBeatRateTable{
            32.0f,
            16.0f,
            8.0f,
            4.0f,
            2.0f,
            1.0f,
            1.0f / 2.0f,
            1.0f / 4.0f,
            1.0f / 8.0f,
            1.0f / 16.0f,
            1.0f / 32.0f
        };
        static_assert(kBeatRateTable.size() == param::LFO_BeatRate::kNames.size());
        auto acc_rate = 1.0f / kBeatRateTable[param::LFO_BeatRate::GetChoiceIndex(raw_rate)];
        rate = acc_rate * beat_rate;
    }
    break;
    default:
        assert(false);
        break;
    }

    auto p_inc = rate * inv_update_rate_;
    lfo_phase_ += p_inc;

    auto mode = param::LFO_Mode::GetEnum(lfo_mode_->GetInt());
    using mt = param::LFO_Mode::ParamEnum;
    switch (mode) {
    case mt::kOnce:
        lfo_phase_ = std::min(1.0f, lfo_phase_);
        break;
    default:
    {
        float tmp{};
        lfo_phase_ = std::modf(lfo_phase_, &tmp);
        break;
    }
    }

    auto wave_type = param::LFO_WaveType::GetEnum(wave_type_->GetInt());
    using wt = param::LFO_WaveType::ParamEnum;
    switch (wave_type) {
    case wt::kSine:
        output_value_ = utli::warp::AtNormalizeIndex(kSineTable, lfo_phase_);
        break;
    case wt::kTri:
        output_value_ = std::abs(2.0f * lfo_phase_ - 1.0f);
        break;
    case wt::kSawUp:
        output_value_ = lfo_phase_;
        break;
    case wt::kSawDown:
        output_value_ = 1.0f - lfo_phase_;
        break;
    case wt::kSquare:
        output_value_ = lfo_phase_ > 0.5f ? 1.0f : 0.0f;
        break;
    case wt::kCustom:
        output_value_ = wave_curve_->GetNormalize(lfo_phase_);
        break;
    default:
        assert(false);
        break;
    }
}

void LFO::OnNoteOn(int note) {
    auto mode = param::LFO_Mode::GetEnum(lfo_mode_->GetInt());
    using mt = param::LFO_Mode::ParamEnum;
    switch (mode) {
    case mt::kReset:
    case mt::kOnce:
        lfo_phase_ = start_phase_->GetValue();
        break;
    case mt::kFree:
    default:
        break;
    }
}
}