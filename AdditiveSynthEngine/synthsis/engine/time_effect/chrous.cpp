#include "chrous.h"

#include "param/timefx/chrous_param.h"
#include "utli/convert.h"

namespace mana {
void mana::Chrous::Init(float sample_rate, float update_rate) {
    auto max_time_ms = param::Chrous_Delay1::kMax + param::Chrous_Depth::kMax;
    auto num_samples = static_cast<int>(max_time_ms * sample_rate / 1000.0f);
    delay1_.SetMaxSize(num_samples);
    delay1_samples_.SetSmooth(sample_rate, 100.0f);
    delay2_.SetMaxSize(num_samples);
    delay2_samples_.SetSmooth(sample_rate, 100.0f);
    sample_rate_ = sample_rate;
    smooth_feedback_.SetSmooth(sample_rate, 50.0f);
    chorus_lfo_.Init(update_rate);
}

void Chrous::PrepareParams(ModulableParams& params) {
    enable_ = params.GetParam<BoolParameter>("timefx.chrous.enable");
    rate_mode_ = params.GetParam<IntChoiceParameter>(param::Chrous_RateMode::kId);
    rate_ = params.GetModuFloatParam("timefx.chrous.rate");
    bpm_ = params.GetParam<FloatParameter>("bpm");
    delay1_time_ = params.GetModuFloatParam(param::Chrous_Delay1::kId);
    delay2_time_ = params.GetModuFloatParam(param::Chrous_Delay2::kId);
    depth_ = params.GetModuFloatParam(param::Chrous_Depth::kId);
    feedback_ = params.GetModuFloatParam(param::Chrous_Feedback::kId);
}

void Chrous::OnUpdateTick() {
    auto rate_mode = param::Chrous_RateMode::GetEnum(rate_mode_->GetInt());
    using rm = param::Chrous_RateMode::ParamEnum;
    auto rate = 0.0f;
    switch (rate_mode) {
    case rm::kHz:
    {
        rate = param::Chrous_RateHz::ConvertFrom01(rate_->GetValue());
        break;
    }
    case rm::kBeats:
    {
        auto idx = param::Chrous_RateBeat::GetChoiceIndexFrom01(rate_->GetValue());
        auto beats = param::Chrous_RateBeat::kNumbers[idx];
        auto sec = utli::BpmToSeconds(bpm_->GetValue(), beats);
        rate = 1.0f / sec;
        break;
    }
    default:
        assert(false);
        break;
    }
    chorus_lfo_.SetFreq(rate);
    chorus_lfo_.ResetGain();
    auto delay1_time = delay1_time_->GetValue();
    auto delay2_time = delay2_time_->GetValue();
    auto depth = depth_->GetValue();
    chorus_lfo_.TickNext();
    auto dd1 = 0.5f + 0.5f * chorus_lfo_.Sine();
    auto dd2 = 0.5f + 0.5f * chorus_lfo_.Cosine();
    auto d1_samples = sample_rate_ / 1000.0f * (delay1_time + depth * dd1);
    auto d2_samples = sample_rate_ / 1000.0f * (delay2_time + depth * dd2);
    delay1_samples_.SetTarget(d1_samples);
    delay2_samples_.SetTarget(d2_samples);

    smooth_feedback_.SetTarget(feedback_->GetValue());
}

void Chrous::Process(float* pbuffer, int num) {
    if (!enable_->GetBool()) return;

    for (int i = 0; i < num; i++) {
        auto fb_val = last_sample_ * smooth_feedback_.TickNext();
        auto d1 = delay1_.Process(pbuffer[i] + fb_val, delay1_samples_.TickNext());
        auto d2 = delay2_.Process(pbuffer[i] + fb_val, delay2_samples_.TickNext());
        pbuffer[i] = d2 + d1 + pbuffer[i];
        last_sample_ = (d1 + d2) * 0.5f;
    }
}
}