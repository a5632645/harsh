#include "dissonance.h"

#include <numbers>
#include "utli/warp.h"
#include "engine/oscillor_param.h"
#include "param/dissonance_param.h"
#include "utli/convert.h"
#include "param/param_helper.h"

namespace mana {
static void DoHarmonicStretch(Partials& partials, float stretch) {
    auto ratio = 1.0f;
    auto base_freq = partials.base_frequency;

    for (int i = 0; i < kNumPartials; ++i) {
        auto final_ratio = ratio + partials.ratios[i];
        partials.freqs[i] = base_freq * final_ratio;
        partials.pitches[i] = utli::RatioToPitch(final_ratio, partials.base_pitch);
        partials.ratios[i] = final_ratio;
        ratio += stretch;
    }
}

static void DoSemitoneSpace(Partials& partials, float semitone) {
    auto ratio = 1.0f;
    auto ratio_mul = std::exp2(semitone / 12.0f);
    auto base_freq = partials.base_frequency;

    int i = 0;
    for (i = 0; i < kNumPartials && ratio < kNumPartials * 8.0f; ++i) {
        auto final_ratio = ratio + partials.ratios[i];
        partials.freqs[i] = final_ratio * base_freq;
        partials.pitches[i] = utli::RatioToPitch(final_ratio, partials.base_pitch);
        partials.ratios[i] = final_ratio;
        ratio *= ratio_mul;
    }
    for (; i < kNumPartials; ++i) {
        partials.ratios[i] = 0.0f;
        partials.freqs[i] = -1.0f;
        partials.pitches[i] = 0.0f;
        partials.gains[i] = 0.0f;
    }
}

void Dissonance::DoStringDiss(Partials& partials) {
    auto string_val = helper::GetAlterParamValue(args_, param::StringDissStretch{});
    string_val = (std::exp(string_val * 10.0f) - 1.0f) / (gcem::exp(10.0f) - 1.0f);
    auto string_quantize = helper::GetAlterParamValue(args_, param::String_Quantize{});

    auto first_ratio = partials.ratios[0] + 1.0f;
    partials.freqs[0] = first_ratio * partials.base_frequency;
    partials.pitches[0] = utli::RatioToPitch(first_ratio, partials.base_pitch);
    partials.ratios[0] = first_ratio;

    for (int i = 1; i < kNumPartials; ++i) {
        auto n = i + 1.0f;
        auto str_ratio = n * std::sqrt(1.0f + n * n * string_val) + partials.ratios[i];
        auto quantize_ratio = std::round(str_ratio);
        auto ratio = std::lerp(str_ratio, quantize_ratio, string_quantize);
        partials.freqs[i] = partials.base_frequency * ratio;
        partials.pitches[i] = utli::RatioToPitch(ratio, partials.base_pitch);
        partials.ratios[i] = ratio;
    }
}

// =========================================================
// prism
//   prism from harmor, in harmor prism
//   add mode:    partial's ratios will add +-4
//   multi mode:  (n)th partial will shift from 1 to ratio=(2n+1), n=0,1,2...
// =========================================================
static constexpr auto kMaxPrismAddRatio = 4.0f;
void Dissonance::DoPrism(Partials& partials) {
    auto prism_amount = helper::GetAlterParamValue(args_, param::Prism_Amount{}); // -1..1
    auto prism_morph = helper::GetAlterParamValue(args_, param::Prism_Morph{});
    for (int i = 0; i < kNumPartials; ++i) {
        auto org_ratio = partials.ratios[i] + 1.0f + i;
        auto freq = partials.base_frequency * org_ratio;
        if (freq > 0.0f && freq < Partials::kMaxFreq) {
            auto multi_ratio1 = 2.0f * i + 1.0f + partials.ratios[i];
            auto multi_ratio0 = 1.0f + partials.ratios[i];
            // mel frequency transform
            auto mel = 1127.0f * std::log(1.0f + freq / 700.0f);
            constexpr auto max_mel = 1127.0f * gcem::log(1.0f + Partials::kMaxFreq / 700.0f);
            constexpr auto inv_max_mel = 1.0f / max_mel;
            auto prism_val = prism_map_->GetNormalize(inv_max_mel * mel); // 0..1
            prism_val = prism_val * 2.0f - 1.0f; // -1..1
            auto add_ratio = org_ratio + prism_val * prism_amount * kMaxPrismAddRatio;
            auto bit_prism_val = prism_val * prism_amount * 0.5f + 0.5f; // 0..1
            auto multi_ratio = std::lerp(multi_ratio0, multi_ratio1, bit_prism_val);
            auto ratio = std::lerp(add_ratio, multi_ratio, prism_morph);
            partials.freqs[i] = partials.base_frequency * ratio;
            partials.pitches[i] = utli::RatioToPitch(ratio, partials.base_pitch);
            partials.ratios[i] = ratio;
        }
        else {
            partials.freqs[i] = freq;
            partials.pitches[i] = utli::RatioToPitch(org_ratio, partials.base_pitch);
            partials.ratios[i] = org_ratio;
        }
    }
}

// =========================================================
// dispersion
// =========================================================
static void DoDispersion(Partials& partials, float amount, float warp) {
    auto first_ratio = partials.ratios[0] + 1.0f;
    partials.freqs[0] = first_ratio * partials.base_frequency;
    partials.pitches[0] = utli::RatioToPitch(first_ratio, partials.base_pitch);
    partials.ratios[0] = first_ratio;

    if (amount < 0.0f) {
        for (int i = 1; i < kNumPartials; ++i) {
            auto idx01 = static_cast<float>(i) / static_cast<float>(kNumPartials);
            auto spread = 1.0f + utli::warp::ParabolaWarp(idx01, warp) * std::abs(amount);
            auto ratio = (i + 1.0f) / spread + partials.ratios[i];
            partials.freqs[i] = partials.base_frequency * ratio;
            partials.pitches[i] = utli::RatioToPitch(ratio, partials.base_pitch);
            partials.ratios[i] = ratio;
        }
    }
    else {
        for (int i = 1; i < kNumPartials; ++i) {
            auto idx01 = static_cast<float>(i) / static_cast<float>(kNumPartials);
            auto spread = 1.0f + utli::warp::ParabolaWarp(idx01, warp) * std::abs(amount);
            auto ratio = (i + 1.0f) * spread + partials.ratios[i];
            partials.freqs[i] = partials.base_frequency * ratio;
            partials.pitches[i] = utli::RatioToPitch(ratio, partials.base_pitch);
            partials.ratios[i] = ratio;
        }
    }
}

// =========================================================
// noise
// =========================================================
void Dissonance::DoSyncNoise(Partials& partials) {
    auto first_ratio = partials.ratios[0] + 1.0f;
    partials.freqs[0] = first_ratio * partials.base_frequency;
    partials.pitches[0] = utli::RatioToPitch(first_ratio, partials.base_pitch);
    partials.ratios[0] = first_ratio;

    for (int i = 1; i < kNumPartials; ++i) {
        auto org_ratio = i + 1.0f + partials.ratios[i];
        auto ramp_k = (1.0f - error_ramp_) / (kNumPartials - 1);
        auto ramp_mult = error_ramp_ + ramp_k * i;
        auto ratio_mult = ramp_mult * error_range_;
        auto error_ratio = ratio_mult * static_noise_[i];
        auto final_ratio = error_ratio + org_ratio;
        partials.freqs[i] = partials.base_frequency * final_ratio;
        partials.pitches[i] = utli::RatioToPitch(final_ratio, partials.base_pitch);
        partials.ratios[i] = final_ratio;
    }
}

// =========================================================
// fake unison
// =========================================================
static void DoFakeUnison(Partials& partials, float ratio0, float ratio1) {
    for (int i = 0; i < kNumPartials; i += 3) {
        auto ratio = i + 1.0f + partials.ratios[i];
        partials.freqs[i] = partials.base_frequency * ratio;
        partials.pitches[i] = utli::RatioToPitch(ratio, partials.base_pitch);
        partials.ratios[i] = ratio;
    }

    for (int i = 1; i < kNumPartials; i += 3) {
        auto ratio = i + 1.0f + partials.ratios[i];
        auto detuned_ratio = ratio * ratio0;
        partials.freqs[i] = partials.base_frequency * detuned_ratio;
        partials.pitches[i] = utli::RatioToPitch(detuned_ratio, partials.base_pitch);
        partials.ratios[i] = ratio;
    }

    for (int i = 2; i < kNumPartials; i += 3) {
        auto ratio = i + 1.0f + partials.ratios[i];
        auto detuned_ratio = ratio * ratio1;
        partials.freqs[i] = partials.base_frequency * detuned_ratio;
        partials.pitches[i] = utli::RatioToPitch(detuned_ratio, partials.base_pitch);
        partials.ratios[i] = ratio;
    }
}

static void DoFakeUnison2(Partials& partials, float ratio0, float ratio1) {
    int harmonic_idx = 1;
    for (int i = 0; i < kNumPartials; i += 3) {
        auto ratio = harmonic_idx + partials.ratios[i];
        partials.freqs[i] = partials.base_frequency * ratio;
        partials.pitches[i] = utli::RatioToPitch(ratio, partials.base_pitch);
        partials.ratios[i] = ratio;
        ++harmonic_idx;
    }

    harmonic_idx = 1;
    for (int i = 1; i < kNumPartials; i += 3) {
        auto ratio = harmonic_idx + partials.ratios[i];
        auto detuned_ratio = ratio * ratio0;
        partials.freqs[i] = partials.base_frequency * detuned_ratio;
        partials.pitches[i] = utli::RatioToPitch(detuned_ratio, partials.base_pitch);
        partials.ratios[i] = ratio;
        ++harmonic_idx;
    }

    harmonic_idx = 1;
    for (int i = 2; i < kNumPartials; i += 3) {
        auto ratio = harmonic_idx + partials.ratios[i];
        auto detuned_ratio = ratio * ratio1;
        partials.freqs[i] = partials.base_frequency * detuned_ratio;
        partials.pitches[i] = utli::RatioToPitch(detuned_ratio, partials.base_pitch);
        partials.ratios[i] = ratio;
        ++harmonic_idx;
    }
}

// =========================================================
// dissonance
// =========================================================
void Dissonance::Init(float sample_rate, float update_rate) {
}

void Dissonance::PrepareParams(OscillorParams& params) {
    auto& cb = params.GetParentSynthParams().GetCurveBank();
    pitch_quantize_map_ = cb.GetQuantizeMapPtr("dissonance.pitch_quantize");
    prism_map_ = cb.GetCurvePtr("dissonance.prism");

    is_enable_param_ = params.GetParam<BoolParameter>("dissonance.enable");
    diss_type_ = params.GetParam<IntChoiceParameter>("dissonance.type");

    for (int i = 0; auto & arg : args_) {
        arg = params.GetPolyFloatParam("dissonance.arg{}", i++);
    }
}

void Dissonance::Process(Partials& partials) {
    if (!is_enable_) {
        for (int i = 0; i < kNumPartials; ++i) {
            auto ratio = i + 1.0f + partials.ratios[i];
            partials.freqs[i] = ratio * partials.base_frequency;
            partials.pitches[i] = utli::RatioToPitch(ratio, partials.base_pitch);
            partials.ratios[i] = ratio;
        }
        return;
    }

    using enum param::DissonanceType::ParamEnum;
    auto diss_type = param::DissonanceType::GetEnum(diss_type_->GetInt());
    switch (diss_type) {
    case kString:
        DoStringDiss(partials);
        break;
    case kHarmonicStretch:
        DoHarmonicStretch(partials, harmonic_stretch_ratio_);
        break;
    case kSemitoneSpace:
        DoSemitoneSpace(partials, st_space_semitone_);
        break;
    case kStaticError:
        DoSyncNoise(partials);
        break;
    case kFakeUnison:
        DoFakeUnison(partials, ratio2x_ratio_, ratio3x_ratio_);
        break;
    case kFakeUnison2:
        DoFakeUnison2(partials, ratio2x_ratio_, ratio3x_ratio_);
        break;
    case kDispersion:
        DoDispersion(partials, dispersion_amount_, dispersion_warp_);
        break;
    case kPitchQuantize:
        DoPitchQuantize(partials);
        break;
    case kPrism:
        DoPrism(partials);
        break;
    default:
        break;
    }
}

void Dissonance::OnUpdateTick() {
    is_enable_ = is_enable_param_->GetBool();

    harmonic_stretch_ratio_ = param::HarmonicStrech::GetNumber(args_[param::HarmonicStrech::kArgIdx]->Get01Value());

    st_space_semitone_ = param::SemitoneSpace::GetNumber(args_[param::SemitoneSpace::kArgIdx]->Get01Value());

    {
        error_ramp_ = param::ErrorRamp::GetNumber(args_[param::ErrorRamp::kArgIdx]->Get01Value());
        error_range_ = param::ErrorRange::GetNumber(args_[param::ErrorRange::kArgIdx]->Get01Value());
    }

    {
        ratio2x_ratio_ = std::exp2(param::FakeUnisonRatio0::GetNumber(args_[param::FakeUnisonRatio0::kArgIdx]->Get01Value()) / 12.0f);
        ratio3x_ratio_ = std::exp2(param::FakeUnisonRatio1::GetNumber(args_[param::FakeUnisonRatio1::kArgIdx]->Get01Value()) / 12.0f);
    }

    {
        dispersion_amount_ = param::Dispersion_Amount::GetNumber(args_[param::Dispersion_Amount::kArgIdx]->Get01Value());
        dispersion_warp_ = param::Dispersion_Warp::GetNumber(args_[param::Dispersion_Warp::kArgIdx]->Get01Value());
    }
}

void Dissonance::OnNoteOn(int note) {
    for (auto& s : static_noise_) {
        auto v = random_dist_(random_);
        s = 2.0f * v - 1.0f;
    }
}

void Dissonance::OnNoteOff() {
}

void Dissonance::DoPitchQuantize(Partials& partials) {
    float amount = param::PitchQuantize_Amount::GetNumber(args_[param::PitchQuantize_Amount::kArgIdx]->Get01Value());

    for (int i = 0; i < kNumPartials; ++i) {
        float ratio = i + 1.0f + partials.ratios[i];
        if (ratio < 0.0f) {
            partials.freqs[i] = -1.0f;
            partials.pitches[i] = -1.0f;
            partials.ratios[i] = ratio;
            continue;
        }

        float raw_pitch = utli::RatioToPitch(ratio, partials.base_pitch);
        int quantize_pitch = static_cast<int>(std::round(raw_pitch));

        // take apart into octave * 12 + semitone
        int octave = quantize_pitch / 12;
        int semitone = quantize_pitch % 12;
        if (semitone < 0) {
            semitone += 12;
            ++octave;
        }

        // remap semitone
        semitone = static_cast<int>(pitch_quantize_map_->Get(semitone));

        float final_quantize_pitch = octave * 12.0f + semitone;
        float final_pitch = std::lerp(raw_pitch, final_quantize_pitch, amount);
        partials.freqs[i] = partials.base_frequency * std::exp2((final_pitch - partials.base_pitch) / 12.0f);
        partials.pitches[i] = final_pitch;
        partials.ratios[i] = partials.freqs[i] / partials.base_frequency;
    }
}
}