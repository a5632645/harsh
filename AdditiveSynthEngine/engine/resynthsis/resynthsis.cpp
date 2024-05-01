#include "resynthsis.h"

#include "engine/synth.h"
#include "param/resynthsis_param.h"

namespace mana {
void Resynthesis::Process(Partials& partials) {
    if (!synth_.IsResynthsisAvailable()) {
        return;
    }

    auto frame_idx = static_cast<size_t>((synth_.GetResynthsisFrames().frames.size() - 1) * frame_pos_);
    const auto& frame = synth_.GetResynthsisFrames().frames.at(frame_idx);

    std::array<float, kNumPartials> gains{};
    std::ranges::transform(frame.gains, gains.begin(), [this](float v) {return v * gain_makeup_; });

    auto abs_gain_mix = std::abs(gain_mix_);
    if (gain_mix_ < 0.0f) {
        // do vocoder
        std::ranges::transform(partials.gains, gains, partials.gains.begin(), 
                               [abs_gain_mix](float osc_gain, float res_gain) {
            return std::lerp(osc_gain, res_gain * osc_gain, abs_gain_mix);
        });
    }
    else {
        // do dry wet mix
        std::ranges::transform(partials.gains, gains, partials.gains.begin(),
                               [abs_gain_mix](float osc_gain, float res_gain) {
            return std::lerp(osc_gain, res_gain, abs_gain_mix);
        });
    }
}

void Resynthesis::OnUpdateTick(const SynthParam& params, int skip, int module_idx) {
    formant_mix_ = param::Resynthsis_FormantMix::GetNumber(params.resynthsis.args);
    formant_shift_ = param::Resynthsis_FormantShift::GetNumber(params.resynthsis.args);
    freq_scale_ = param::Resynthsis_FreqScale::GetNumber(params.resynthsis.args);
    frame_offset_ = param::Resynthsis_FrameOffset::GetNumber(params.resynthsis.args);
    frame_speed_ = param::Resynthsis_FrameSpeed::GetNumber(params.resynthsis.args);
    gain_mix_ = param::Resynthsis_GainMix::GetNumber(params.resynthsis.args);
    gain_makeup_ = Db2Gain(param::Resynthsis_MakeUp::GetNumber(params.resynthsis.args));

    if (!synth_.IsResynthsisAvailable()) return;

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
    if (!synth_.IsResynthsisAvailable() || freq_scale_ == float{}) {
        partials.freqs.fill(0.0f);
        return;
    }

    auto frame_idx = static_cast<size_t>((synth_.GetResynthsisFrames().frames.size() - 1) * frame_pos_);
    const auto& frame = synth_.GetResynthsisFrames().frames.at(frame_idx);
    auto ratio_scale = freq_scale_ / synth_.GetResynthsisFrames().data_series_freq;
    std::ranges::transform(frame.freq_diffs, partials.freqs.begin(),
                           [ratio_scale](float fre_diff) {return ratio_scale * fre_diff; });
}

void Resynthesis::OnNoteOn(int /*note*/) {
    frame_player_pos_ = float{};
}
}