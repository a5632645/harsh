#include "resynthsis.h"

#include "engine/synth.h"
#include "param/resynthsis_param.h"
#include "utli/warp.h"

namespace mana {
void Resynthesis::Init(float sample_rate, float update_rate) {
    sample_rate_ = sample_rate;
    skip_ = sample_rate / update_rate;
}

void Resynthesis::Process(Partials& partials) {
    if (!IsWork()) {
        return;
    }

    auto frame_idx = static_cast<size_t>((synth_.GetResynthsisFrames().frames.size() - 1) * frame_pos_);
    const auto& frame = synth_.GetResynthsisFrames().frames.at(frame_idx);

    std::array<float, kNumPartials> gains{};
    using namespace std::views;

    if (formant_mix_ != float{}) {
        auto formant_gains = GetFormantGains(partials, frame);
        std::ranges::transform(frame.gains | take(kNumPartials), formant_gains, gains.begin(),
                               [this](float no_shift_gain, float shift_gain) {
            return std::lerp(no_shift_gain, shift_gain, formant_mix_);
        });
    }
    else {
        std::ranges::copy(frame.gains | take(kNumPartials), gains.begin());
    }

    auto osc_mix = 1.0f - gain_mix_;
    auto res_mix = gain_mix_;
    std::ranges::transform(partials.gains, gains, partials.gains.begin(),
                           [osc_mix, res_mix](float osc_gain, float res_gain) {
        return osc_mix * osc_gain + res_mix * res_gain;
    });
}

void Resynthesis::OnUpdateTick() {
    is_enable_ = is_enable_arg_->GetBool();

    formant_mix_ = param::Resynthsis_FormantMix::GetNumber(args_);
    formant_shift_ = param::Resynthsis_FormantShift::GetNumber(args_);
    freq_scale_ = param::Resynthsis_FreqScale::GetNumber(args_);
    frame_offset_ = param::Resynthsis_FrameOffset::GetNumber(args_);
    frame_speed_ = param::Resynthsis_FrameSpeed::GetNumber(args_);
    gain_mix_ = param::Resynthsis_GainMix::GetNumber(args_);

    if (!IsWork()) return;

    auto pos_inc = frame_speed_ * (float)skip_ / synth_.GetResynthsisFrames().frame_interval_sample / synth_.GetResynthsisFrames().frames.size();
    frame_player_pos_ += pos_inc;
    while (frame_player_pos_ > 1.0f) {
        frame_player_pos_ -= 1.0f;
    }
    while (frame_player_pos_ < 0.0f) {
        frame_player_pos_ += 1.0f;
    }

    frame_pos_ = frame_player_pos_ + frame_offset_;
    while (frame_pos_ > 1.0f) {
        frame_pos_ -= 1.0f;
    }
}

void Resynthesis::PrepareParams(OscillorParams & params) {
    formant_remap_curve_ = params.GetParentSynthParams().GetCurveManager().GetCurvePtr("resynthsis.formant_remap");

    is_enable_arg_ = params.GetParam<BoolParameter>("resynthsis.enable");
    is_formant_remap_ = params.GetParam<BoolParameter>("resynthsis.formant_remap");
    for (int i = 0; auto & parg : args_) {
        parg = params.GetPolyFloatParam("resynthsis.arg{}", i++);
    }
}

void Resynthesis::PreGetFreqDiffsInRatio(Partials& partials) {
    if (!is_enable_ || !synth_.IsResynthsisAvailable() || freq_scale_ == float{}) {
        partials.ratios.fill(0.0f);
        return;
    }

    using namespace std::views;

    auto frame_idx = static_cast<size_t>((synth_.GetResynthsisFrames().frames.size() - 1) * frame_pos_);
    const auto& frame = synth_.GetResynthsisFrames().frames.at(frame_idx);
    std::ranges::transform(frame.ratio_diffs | take(kNumPartials), partials.ratios.begin(),
                           [this](float ratio_diff) {return ratio_diff * freq_scale_; });
}

bool Resynthesis::IsWork() const {
    return is_running_ && is_enable_ && synth_.IsResynthsisAvailable();
}

std::array<float, kNumPartials> Resynthesis::GetFormantGains(Partials& partials,
                                                             const ResynthsisFrames::FftFrame& frame) const {
    auto formant_ratio = std::exp2(-formant_shift_ / 12.0f);
    std::array<float, kNumPartials> output{};

    if (!is_formant_remap_->GetBool()) { // disable remap
        for (int i = 0; i < kNumPartials; ++i) {
            auto idx = partials.freqs[i] * (kFFtSize / 2) * formant_ratio - 1.0f;
            auto int_idx = static_cast<int>(std::round(idx));

            if (int_idx < 0 || int_idx >= kFFtSize / 2) {
                output[i] = 0.0f;
            }
            else {
                output[i] = frame.gains[int_idx];
            }
        }
    }
    else {
        for (int i = 0; i < kNumPartials; ++i) {
            auto idx = partials.freqs[i] * (kFFtSize / 2) * formant_ratio - 1.0f;
            //auto int_idx = static_cast<int>(std::round(idx));
            auto norm_idx = idx / static_cast<float>(kFFtSize / 2);
            if (norm_idx < 0.0f || norm_idx > 1.0f) {
                output[i] = 0.0f;
                continue;
            }

            auto remap_norm_idx = utli::warp::AtNormalizeIndex(formant_remap_curve_->data, norm_idx);
            auto remap_idx = static_cast<int>(std::round(remap_norm_idx * kFFtSize / 2));

            if (remap_idx < 0 || remap_idx >= kFFtSize / 2) {
                output[i] = 0.0f;
            }
            else {
                output[i] = frame.gains[remap_idx];
            }
        }
    }

    return output;
}

void Resynthesis::OnNoteOn(int /*note*/) {
    frame_player_pos_ = float{};
    is_running_ = true;
}
}