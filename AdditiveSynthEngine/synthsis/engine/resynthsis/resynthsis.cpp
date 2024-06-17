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
    formant_remap_curve_ = params.GetParentSynthParams().GetCurveBank().GetCurvePtr("resynthsis.formant_remap");
    pos_offset_curve_ = params.GetParentSynthParams().GetCurveBank().GetCurvePtr("resynthsis.rand_start_pos_mask");
    speed_curve_ = params.GetParentSynthParams().GetCurveBank().GetCurvePtr("resynthsis.speed");

    is_enable_ = params.GetParam<BoolParameter>("resynthsis.enable");
    is_formant_remap_ = params.GetParam<BoolParameter>("resynthsis.formant_remap");
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
    if (!is_formant_remap_->GetBool()) { // disable remap
        for (int i = 0; i < kNumPartials; ++i) {
            auto remap_freq = partials.freqs[i] * 0.5f * sample_rate_ * formant_ratio;
            auto formant_max_freq = (kNumPartials + 1.0f) * resynthsis_datas.base_freq;
            auto formant_data_max_freq = kNumPartials * resynthsis_datas.base_freq;
            if (remap_freq > formant_max_freq || remap_freq < 0.0f) {
                output[i] = -300.0f;
            }
            else if (remap_freq < resynthsis_datas.base_freq) {
                auto frac = remap_freq / resynthsis_datas.base_freq;
                output[i] = GetFrameGain(i, 0).gain_db * frac;
            }
            else if (remap_freq > formant_data_max_freq) {
                auto frac = 1.0f - (remap_freq - formant_data_max_freq) / resynthsis_datas.base_freq;
                output[i] = GetFrameGain(i, kNumPartials - 1).gain_db * frac;
            }
            else {
                auto index_k = remap_freq / resynthsis_datas.base_freq - 1.0f;
                auto left_idx = static_cast<int>(std::floor(index_k));
                auto right_idx = static_cast<int>(std::ceil(index_k)) & (kNumPartials - 1); // if idx euqal to 256 that is mirror to bin 0
                auto frac = index_k - left_idx;
                output[i] = std::lerp(GetFrameGain(i, left_idx).gain_db, GetFrameGain(i, right_idx).gain_db, frac);
            }
        }
    }
    else { // enable formant remap
        for (int i = 0; i < kNumPartials; ++i) {
            // remap frequency
            auto org_freq = 0.5f * sample_rate_ * partials.freqs[i];
            auto remap_freq = org_freq;
            constexpr auto blend_min = 20.0f;
            constexpr auto blend_max = 20000.0f;
            if (org_freq >= blend_min && org_freq <= blend_max) {
                auto map_input_x = (1.0f / 3.0f) * std::log10(org_freq / blend_min);
                auto map_x = formant_remap_curve_->GetNormalize(map_input_x);
                remap_freq = blend_min * std::exp(std::numbers::ln10_v<float> *map_x * 3.0f);
            }

            // do formant shift
            auto formant_max_freq = (kNumPartials + 1.0f) * resynthsis_datas.base_freq;
            auto formant_data_max_freq = kNumPartials * resynthsis_datas.base_freq;
            if (remap_freq > formant_max_freq || remap_freq < 0.0f) {
                output[i] = -300.0f;
            }
            else if (remap_freq < resynthsis_datas.base_freq) {
                auto frac = remap_freq / resynthsis_datas.base_freq;
                output[i] = GetFrameGain(i, 0).gain_db * frac;
            }
            else if (remap_freq > formant_data_max_freq) {
                auto frac = 1.0f - (remap_freq - formant_data_max_freq) / resynthsis_datas.base_freq;
                output[i] = GetFrameGain(i, kNumPartials - 1).gain_db * frac;
            }
            else {
                auto index_k = remap_freq / resynthsis_datas.base_freq - 1.0f;
                auto left_idx = static_cast<int>(std::floor(index_k));
                auto right_idx = static_cast<int>(std::ceil(index_k)) & (kNumPartials - 1); // if idx euqal to 256 that is mirror to bin 0
                auto frac = index_k - left_idx;
                output[i] = std::lerp(GetFrameGain(i, left_idx).gain_db, GetFrameGain(i, right_idx).gain_db, frac);
            }
        }
    }

    for (auto& g : output) {
        g = utli::DbToGain(g);
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