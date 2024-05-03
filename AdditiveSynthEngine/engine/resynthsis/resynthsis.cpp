#include "resynthsis.h"

#include "engine/synth.h"
#include "param/resynthsis_param.h"

namespace mana {
void Resynthesis::Init(float sample_rate) {
    sample_rate_ = sample_rate;
}

void Resynthesis::Process(Partials& partials) {
    if (!IsWork()) {
        return;
    }

    auto frame_idx = static_cast<size_t>((synth_.GetResynthsisFrames().frames.size() - 1) * frame_pos_);
    const auto& frame = synth_.GetResynthsisFrames().frames.at(frame_idx);

    std::array<float, kNumPartials> gains{};

    if (formant_mix_ != float{}) {
        auto formant_gains = GetFormantGains(partials, frame);
        std::ranges::transform(frame.gains, formant_gains, gains.begin(),
                               [this](float no_shift_gain, float shift_gain) {
            return std::lerp(no_shift_gain, shift_gain, formant_mix_);
        });
    }
    else {
        std::ranges::copy(frame.gains, gains.begin());
    }

    auto gain_dry_wet = gain_mix_;
    std::ranges::transform(partials.gains, gains, partials.gains.begin(),
                           [gain_dry_wet](float osc_gain, float res_gain) {
        return std::lerp(osc_gain, res_gain, gain_dry_wet);
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

    auto pos_inc = frame_speed_ * (float)skip / kFFtHop / synth_.GetResynthsisFrames().frames.size();
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
    if (!IsWork() || freq_scale_ == float{}) {
        partials.freqs.fill(0.0f);
        return;
    }

    auto frame_idx = static_cast<size_t>((synth_.GetResynthsisFrames().frames.size() - 1) * frame_pos_);
    const auto& frame = synth_.GetResynthsisFrames().frames.at(frame_idx);
    auto ratio_scale = freq_scale_ / synth_.GetResynthsisFrames().data_series_freq;
    std::ranges::transform(frame.freq_diffs, partials.freqs.begin(),
                           [ratio_scale](float fre_diff) {return ratio_scale * fre_diff; });
}

bool Resynthesis::IsWork() const {
    return is_running_ && is_enable_ && synth_.IsResynthsisAvailable();
}

std::array<float, kNumPartials> Resynthesis::GetFormantGains(Partials& partials,
                                                             const ResynthsisFrames::FftFrame& frame) const {
    constexpr auto fft_fre_begin = 1.0f / (kFFtSize / 2.0f);
    constexpr auto fft_fre_end = static_cast<float>(kNumPartials) / (kFFtSize / 2.0f);

    auto formant_ratio = std::exp2(formant_shift_ / 12.0f);
    auto formant_begin_fre = formant_ratio * fft_fre_begin;
    auto formant_end_fre = formant_ratio * fft_fre_end;
    auto one_div_range = 1.0f / (formant_end_fre - formant_begin_fre);

    std::array<float, kNumPartials> output{};
    for (int i = 0; i < kNumPartials; ++i) {
        auto fre_ratio = (partials.freqs[i] - formant_begin_fre) * one_div_range;
        if (fre_ratio < 0.0f || fre_ratio > 1.0f) {
            output[i] = 0.0f;
        }
        else {
            auto idx = static_cast<size_t>((frame.freq_diffs.size() - 1) * fre_ratio);
            output[i] = frame.gains[idx];
        }
    }
    return output;
}

void Resynthesis::OnNoteOn(int /*note*/) {
    frame_player_pos_ = float{};
    is_running_ = true;
}
}