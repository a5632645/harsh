#include "distortion.h"

#include "param/timefx/distortion_param.h"
#include "utli/convert.h"

namespace mana {
template<float (*func)(float)>
static void DoDistortion(float* pbuffer, int num, utli::Smoother& amount) {
    for (int i = 0; i < num; ++i) {
        auto gain = utli::DbToGain(amount.TickNext());
        pbuffer[i] = func(pbuffer[i] * gain);
    }
}

static float SoftClip(float in) {
    return std::tanh(in);
}
static float HardClip(float in) {
    return std::clamp(in, -1.0f, 1.0f);
}
}

namespace mana {
void mana::Distortion::Init(float sample_rate, float update_rate) {
    inv_sample_rate_ = 1.0f / sample_rate;
    smooth_dist_amount_.SetSmooth(sample_rate);
    smooth_filter_cutoff_.SetSmooth(sample_rate);
    smooth_filter_mode_.SetSmooth(sample_rate);
}

void Distortion::PrepareParams(ModulableParams& params) {
    enable_ = params.GetParam<BoolParameter>("timefx.distortion.enable");
    dist_type_ = params.GetParam<IntChoiceParameter>(param::DistortionType::kId);
    dist_amount_ = params.GetModuFloatParam(param::DistortionAmount::kId);
    filter_type_ = params.GetParam<IntChoiceParameter>(param::DistortionFilterType::kId);
    filter_cutoff_ = params.GetModuFloatParam(param::DistortionFilterCutoff::kId);
    filter_mode_ = params.GetModuFloatParam(param::DistortionFilterMode::kId);
}

void Distortion::OnUpdateTick() {
    smooth_dist_amount_.SetTarget(dist_amount_->GetValue());
    smooth_filter_cutoff_.SetTarget(std::clamp(utli::PitchToFreq(filter_cutoff_->GetValue()) * inv_sample_rate_, 0.0f, 0.5f));
    smooth_filter_mode_.SetTarget(filter_mode_->GetValue());
}

void Distortion::Process(float* pbuffer, int num) {
    if (!enable_->GetBool()) return;

    auto filter_type = param::DistortionFilterType::GetEnum(filter_type_->GetInt());
    using dft = param::DistortionFilterType::ParamEnum;
    if (filter_type == dft::kPre) {
        for (int i = 0; i < num; ++i) {
            pre_filter_.SetNormalizeCutoff(smooth_filter_cutoff_.TickNext());
            pre_filter_.Process(pbuffer[i]);
            pbuffer[i] = pre_filter_.GetLowpass();
        }
    }

    auto dist_type = param::DistortionType::GetEnum(dist_type_->GetInt());
    using dt = param::DistortionType::ParamEnum;
    switch (dist_type) {
    case dt::kSoftClip:
        DoDistortion<SoftClip>(pbuffer, num, smooth_dist_amount_);
        break;
    case dt::kHardClip:
        DoDistortion<HardClip>(pbuffer, num, smooth_dist_amount_);
        break;
    default:
        assert(false);
        break;
    }

    if (filter_type == dft::kPost) {
        for (int i = 0; i < num; ++i) {
            post_filter_.SetNormalizeCutoff(smooth_filter_cutoff_.TickNext());
            post_filter_.Process(pbuffer[i]);
            pbuffer[i] = post_filter_.GetLowpass();
        }
    }
}
}