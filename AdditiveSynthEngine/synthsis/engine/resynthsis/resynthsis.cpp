#include "resynthsis.h"

#include "engine/synth.h"
#include "param/resynthsis_param.h"
#include "utli/warp.h"
#include "utli/convert.h"
#include "param/param_helper.h"

namespace mana {
void Resynthesis::Init(float sample_rate, float update_rate) {
    sample_rate_ = sample_rate;
    inv_sample_rate_ = 1.0f / sample_rate;
    update_skip_ = sample_rate / update_rate;
}

void Resynthesis::Process(Partials& partials) {
    if (!IsWork()) {
        return;
    }

    std::array<float, kNumPartials> gains{};
    for (int i = 0; i < kNumPartials; ++i) {
        gains[i] = utli::DbToGain(GetFrameGain(i, i).gain_db);
    }

    auto formant_mix = formant_mix_->GetValue();
    if (formant_mix != float{}) {
        auto formant_gains = GetFormantGains(partials);
        std::ranges::transform(gains, formant_gains, gains.begin(),
                               [formant_mix](float no_shift_gain, float shift_gain) {
            return std::lerp(no_shift_gain, shift_gain, formant_mix);
        });
    }

    auto res_mix = gain_mix_->GetValue();
    auto osc_mix = 1.0f - res_mix;
    std::ranges::transform(partials.gains, gains, partials.gains.begin(),
                           [osc_mix, res_mix](float osc_gain, float res_gain) {
        return osc_mix * osc_gain + res_mix * res_gain;
    });
}

void Resynthesis::OnUpdateTick() {
    if (!IsWork())
        return;

    final_speed_ = speed_->GetValue() * speedx_->GetValue();
    auto nor_index = nor_start_pos_->GetValue();
    auto what = update_skip_ / (synth_.GetResynthsisFrames().frame_interval_sample * synth_.GetResynthsisFrames().frames.size());
    for (int i = 0; i < kNumPartials; ++i) {
        auto speed = final_speed_ * speed_curve_->Get(i);
        auto pos_inc = speed * what;
        auto index = partials_runnig_frame_idxs_[i] + pos_inc;
        float tmp{};
        index = std::modf(index, &tmp);
        index = std::modf(index + 1.0f, &tmp);
        partials_runnig_frame_idxs_[i] = index;
        auto final_idx = partials_runnig_frame_idxs_[i] + nor_index + partials_init_frame_idxs_[i];
        final_idx = std::modf(final_idx, &tmp);
        partials_frame_idxs_[i] = final_idx;
    }
}

void Resynthesis::PrepareParams(OscillorParams & params) {
    pos_offset_curve_ = params.GetParentSynthParams().GetCurveBank().GetCurvePtr("resynthsis.rand_start_pos_mask");
    speed_curve_ = params.GetParentSynthParams().GetCurveBank().GetCurvePtr("resynthsis.speed");

    is_enable_ = params.GetParam<BoolParameter>("resynthsis.enable");
    freq_scale_ = params.GetPolyFloatParam("resynthsis.freq_scale");
    nor_start_pos_ = params.GetPolyFloatParam("resynthsis.start_offset");
    speed_ = params.GetPolyFloatParam("resynthsis.speed");
    speedx_ = params.GetPolyFloatParam("resynthsis.speedx");
    formant_mix_ = params.GetPolyFloatParam("resynthsis.formant_mix");
    formant_shift_ = params.GetPolyFloatParam("resynthsis.formant_shift");
    gain_mix_ = params.GetPolyFloatParam("resynthsis.gain_mix");
    partial_start_range_second_ = params.GetParam<FloatParameter>("resynthsis.start_range");
}

void Resynthesis::PreGetFreqDiffsInRatio(Partials& partials) {
    auto f_scale = freq_scale_->GetValue();
    if (!IsWork() || f_scale == float{}) {
        partials.ratios.fill(0.0f);
        return;
    }

    for (int i = 0; i < kNumPartials; ++i) {
        partials.ratios[i] = GetFrameGain(i, i).ratio_diff * f_scale;
    }
}

std::array<float, kNumPartials> Resynthesis::GetFormantGains(Partials& partials) const {
    auto formant_ratio = std::exp2(-formant_shift_->GetValue() / 12.0f);
    std::array<float, kNumPartials> output{};

    const auto& resynthsis_datas = synth_.GetResynthsisFrames();
    const auto inv_base_freq = 1.0f / resynthsis_datas.base_freq;
    const auto max_freq = resynthsis_datas.base_freq * (kNumPartials + 1.0f);
    constexpr auto min_db = -120.0f;
    for (int i = 0; i < kNumPartials; ++i) {
        auto remap_freq = partials.freqs[i] * 0.5f * sample_rate_ * formant_ratio;
        auto first_freq = resynthsis_datas.base_freq * (GetFrameGain(i, 0).ratio_diff + 1.0f);
        auto last_freq = resynthsis_datas.base_freq * (GetFrameGain(i, kNumPartials - 1).ratio_diff + kNumPartials);
        if(remap_freq > max_freq || remap_freq < 0.0f) {
            output[i] = 0.0f;
            continue;
        }
        if (remap_freq < first_freq) {
            auto ratio = remap_freq / first_freq;
            output[i] = utli::DbToGain(std::lerp(min_db, GetFrameGain(i, 0).gain_db, std::abs(ratio)));
            continue;
        }
        if (remap_freq > last_freq) {
            auto ratio = (remap_freq - last_freq) / (max_freq - last_freq);
            output[i] = utli::DbToGain(std::lerp(GetFrameGain(i, kNumPartials - 1).gain_db, min_db, std::abs(ratio)));
            continue;
        }

        auto bin = remap_freq * inv_base_freq - 1.0f;
        auto prev_bin = static_cast<int>(bin);
        auto next_bin = prev_bin + 1;
        if (prev_bin >= kNumPartials || next_bin <= 0) { // out of bound
            output[i] = 0.0f;
            continue;
        }
        if (next_bin == kNumPartials) { // reach end
            auto ratio = (remap_freq - last_freq) / (max_freq - last_freq);
            output[i] = utli::DbToGain(std::lerp(GetFrameGain(i, kNumPartials - 1).gain_db, min_db, std::abs(ratio)));
            continue;
        }

        {
            auto next_bin_freq = resynthsis_datas.base_freq * (GetFrameGain(i, next_bin).ratio_diff + next_bin + 1.0f);
            if (remap_freq > next_bin_freq) {
                ++prev_bin;
                ++next_bin;
            }
            else {
                auto prev_bin_freq = resynthsis_datas.base_freq * (GetFrameGain(i, prev_bin).ratio_diff + prev_bin + 1.0f);
                if (remap_freq < prev_bin_freq) {
                    --prev_bin;
                    --next_bin;
                }
            }
        }
        if (next_bin == kNumPartials) { // reach end
            auto ratio = (remap_freq - last_freq) / (max_freq - last_freq);
            output[i] = utli::DbToGain(std::lerp(GetFrameGain(i, kNumPartials - 1).gain_db, min_db, std::abs(ratio)));
            continue;
        }
        if (next_bin == 0) {
            auto ratio = remap_freq / first_freq;
            output[i] = utli::DbToGain(std::lerp(min_db, GetFrameGain(i, 0).gain_db, std::abs(ratio)));
            continue;
        }

        auto prev_data = GetFrameGain(i, prev_bin);
        auto next_data = GetFrameGain(i, next_bin);
        auto prev_freq = resynthsis_datas.base_freq * (prev_data.ratio_diff + prev_bin + 1.0f);
        auto next_freq = resynthsis_datas.base_freq * (next_data.ratio_diff + next_bin + 1.0f);
        if (prev_freq > next_freq) {
            std::swap(prev_data, next_data);
            std::swap(prev_freq, next_freq);
        }
        if (next_freq - prev_freq < 1.0f) {
            output[i] = utli::DbToGain(prev_data.gain_db);
            continue;
        }
        auto ratio = (remap_freq - prev_freq) / (next_freq - prev_freq);
        output[i] = utli::DbToGain(std::lerp(prev_data.gain_db, next_data.gain_db, std::abs(ratio)));
    }
    return output;
}

void Resynthesis::OnNoteOn(int /*note*/) {
    std::ranges::fill(partials_runnig_frame_idxs_, 0.0f);
    if (synth_.IsResynthsisAvailable()) {
        std::uniform_real_distribution<float> urd{ 0.0, 1.0f };
        if (partial_start_range_second_->GetValue() == param::Resynthsis_StartRange::kMax) { // full random
            for (int i = 0; i < kNumPartials; ++i) {
                partials_init_frame_idxs_[i] = urd(random_);
            }
        }
        else { // according to pos offset map
            auto range_num_samples = partial_start_range_second_->GetValue() * sample_rate_;
            auto& frame = synth_.GetResynthsisFrames();
            auto inv_total_samples = range_num_samples / (frame.frames.size() * frame.frame_interval_sample);
            for (int i = 0; i < kNumPartials; ++i) {
                float tmp{};
                partials_init_frame_idxs_[i] = std::modf(pos_offset_curve_->Get(i) * inv_total_samples * urd(random_), &tmp);
            }
        }
    }
}

Resynthesis::R Resynthesis::GetFrameGain(int partial_idx, int harmonic_idx) const {
    const auto& datas = synth_.GetResynthsisFrames();
    auto float_index = partials_frame_idxs_[partial_idx] * datas.num_frame;
    auto int_index = static_cast<int>(float_index);
    auto next_index = int_index + 1;
    auto frac = float_index - int_index;
    return R{
        .gain_db = std::lerp(datas.frames[int_index].db_gains[harmonic_idx], datas.frames[next_index].db_gains[harmonic_idx], frac),
        .ratio_diff = std::lerp(datas.frames[int_index].ratio_diffs[harmonic_idx], datas.frames[next_index].ratio_diffs[harmonic_idx], frac),
        //.ratio_diff = datas.frames[int_index].ratio_diffs[harmonic_idx]
    };
}

bool Resynthesis::IsWork() const {
    return is_enable_->GetBool() && synth_.IsResynthsisAvailable();
}
}