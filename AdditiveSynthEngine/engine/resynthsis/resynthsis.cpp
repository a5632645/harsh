#include "resynthsis.h"

#include "engine/synth.h"
#include "param/resynthsis_param.h"

namespace mana {
void Resynthesis::Init(float sample_rate, float update_rate) {
    sample_rate_ = sample_rate;
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

void Resynthesis::OnUpdateTick(const SynthParam& params, int skip, int module_idx) {
    formant_mix_ = param::Resynthsis_FormantMix::GetNumber(params.resynthsis.args);
    formant_shift_ = param::Resynthsis_FormantShift::GetNumber(params.resynthsis.args);
    freq_scale_ = param::Resynthsis_FreqScale::GetNumber(params.resynthsis.args);
    frame_offset_ = param::Resynthsis_FrameOffset::GetNumber(params.resynthsis.args);
    frame_speed_ = param::Resynthsis_FrameSpeed::GetNumber(params.resynthsis.args);
    gain_mix_ = param::Resynthsis_GainMix::GetNumber(params.resynthsis.args);
    is_enable_ = params.resynthsis.is_enable;

    if (!IsWork()) return;

    auto pos_inc = frame_speed_ * (float)skip / synth_.GetResynthsisFrames().frame_interval_sample / synth_.GetResynthsisFrames().frames.size();
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
    auto formant_ratio = std::exp2(formant_shift_ / 12.0f);
    auto idx_scale = synth_.GetResynthsisFrames().base_freq / (partials.base_frequency * formant_ratio);

    // Todo: solve formant shift problem
    std::array<float, kNumPartials> output{};
    for (int i = 0; i < kNumPartials; ++i) {
        auto scaled_idx = idx_scale * (i + 1.0f) - 1.0f;
        auto int_idx = static_cast<int>(scaled_idx);
        constexpr auto max_idx = kFFtSize / 2 - 1;
        if (int_idx < 0 || int_idx > max_idx) {
            output[i] = 0.0f;
        }
        else {
            output[i] = frame.gains[int_idx];
        }
    }
    return output;
}

void Resynthesis::OnNoteOn(int /*note*/) {
    frame_player_pos_ = float{};
    is_running_ = true;
}
}