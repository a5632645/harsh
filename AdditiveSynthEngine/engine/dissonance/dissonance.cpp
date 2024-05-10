#include "dissonance.h"

#include "utli/warp.h"

static float RatioToPitch(float ratio, float base_pitch) {
    ratio = std::max(ratio, 0.0001f);
    return base_pitch + 12.0f * std::log2(ratio);
}

namespace mana {
static void DoHarmonicStretch(Partials& partials, float stretch) {
    auto ratio = 1.0f;
    auto base_freq = partials.base_frequency;

    for (int i = 0; i < kNumPartials; ++i) {
        auto final_ratio = ratio + partials.ratios[i];
        partials.freqs[i] = base_freq * final_ratio;
        partials.pitches[i] = RatioToPitch(final_ratio, partials.base_pitch);
        ratio += stretch;
    }
}

static void DoSemitoneSpace(Partials& partials, float semitone) {
    auto ratio = 1.0f;
    auto ratio_mul = std::exp2(semitone / 12.0f);
    auto base_freq = partials.base_frequency;

    for (int i = 0; i < kNumPartials; ++i) {
        auto final_ratio = ratio + partials.ratios[i];
        partials.freqs[i] = final_ratio * base_freq;
        partials.pitches[i] = RatioToPitch(final_ratio, partials.base_pitch);
        ratio *= ratio_mul;
    }
}

static constexpr std::array kStringRatios{ 1.0f, 0.1f,0.01f,0.001f,0.0001f };
static void DoStringDiss(Partials& partials, float string_val) {
    auto first_ratio = partials.ratios[0] + 1.0f;
    partials.freqs[0] = first_ratio * partials.base_frequency;
    partials.pitches[0] = RatioToPitch(first_ratio, partials.base_pitch);

    for (int i = 1; i < kNumPartials; ++i) {
        auto n = i + 1.0f + partials.ratios[i];
        auto ratio = n * std::sqrt(1.0f + n * n * string_val);
        partials.freqs[i] = partials.base_frequency * ratio;
        partials.pitches[i] = RatioToPitch(ratio, partials.base_pitch);
    }
}

// =========================================================
// dispersion
// =========================================================
static void DoDispersion(Partials& partials, float amount, float warp) {
    auto first_ratio = partials.ratios[0] + 1.0f;
    partials.freqs[0] = first_ratio * partials.base_frequency;
    partials.pitches[0] = RatioToPitch(first_ratio, partials.base_pitch);

    if (amount < 0.0f) {
        for (int i = 1; i < kNumPartials; ++i) {
            auto idx01 = static_cast<float>(i) / static_cast<float>(kNumPartials);
            auto spread = 1.0f + utli::warp::ParabolaWarp(idx01, warp) * std::abs(amount);
            auto ratio = (i + 1.0f + partials.ratios[i]) / spread;
            partials.freqs[i] = partials.base_frequency * ratio;
            partials.pitches[i] = RatioToPitch(ratio, partials.base_pitch);
        }
    }
    else {
        for (int i = 1; i < kNumPartials; ++i) {
            auto idx01 = static_cast<float>(i) / static_cast<float>(kNumPartials);
            auto spread = 1.0f + utli::warp::ParabolaWarp(idx01, warp) * std::abs(amount);
            auto ratio = (i + 1.0f + partials.ratios[i]) * spread;
            partials.freqs[i] = partials.base_frequency * ratio;
            partials.pitches[i] = RatioToPitch(ratio, partials.base_pitch);
        }
    }
}

// =========================================================
// noise
// =========================================================
void Dissonance::DoSyncNoise(Partials& partials) {
    auto first_ratio = partials.ratios[0] + 1.0f;
    partials.freqs[0] = first_ratio * partials.base_frequency;
    partials.pitches[0] = RatioToPitch(first_ratio, partials.base_pitch);

    for (int i = 1; i < kNumPartials; ++i) {
        auto org_ratio = i + 1.0f + partials.ratios[i];
        auto ramp_k = (1.0f - error_ramp_) / (kNumPartials - 1);
        auto ramp_mult = error_ramp_ + ramp_k * i;
        auto ratio_mult = ramp_mult * error_range_;
        auto error_ratio = ratio_mult * static_noise_[i];
        auto final_ratio = error_ratio + org_ratio;
        partials.freqs[i] = partials.base_frequency * final_ratio;
        partials.pitches[i] = RatioToPitch(final_ratio, partials.base_pitch);
    }
}

// =========================================================
// fake unison
// =========================================================
static void DoFakeUnison(Partials& partials, float ratio0, float ratio1) {
    for (int i = 0; i < kNumPartials; i += 3) {
        auto ratio = i + 1.0f + partials.ratios[i];
        partials.freqs[i] = partials.base_frequency * ratio;
        partials.pitches[i] = RatioToPitch(ratio, partials.base_pitch);
    }

    for (int i = 1; i < kNumPartials; i += 3) {
        auto ratio = i + 1.0f + partials.ratios[i];
        auto detuned_ratio = ratio * ratio0;
        partials.freqs[i] = partials.base_frequency * detuned_ratio;
        partials.pitches[i] = RatioToPitch(detuned_ratio, partials.base_pitch);
    }

    for (int i = 2; i < kNumPartials; i += 3) {
        auto ratio = i + 1.0f + partials.ratios[i];
        auto detuned_ratio = ratio * ratio1;
        partials.freqs[i] = partials.base_frequency * detuned_ratio;
        partials.pitches[i] = RatioToPitch(detuned_ratio, partials.base_pitch);
    }
}

static void DoFakeUnison2(Partials& partials, float ratio0, float ratio1) {
    int harmonic_idx = 1;
    for (int i = 0; i < kNumPartials; i += 3) {
        auto ratio = harmonic_idx + partials.ratios[i];
        partials.freqs[i] = partials.base_frequency * ratio;
        partials.pitches[i] = RatioToPitch(ratio, partials.base_pitch);
        ++harmonic_idx;
    }

    harmonic_idx = 1;
    for (int i = 1; i < kNumPartials; i += 3) {
        auto ratio = harmonic_idx + partials.ratios[i];
        auto detuned_ratio = ratio * ratio0;
        partials.freqs[i] = partials.base_frequency * detuned_ratio;
        partials.pitches[i] = RatioToPitch(detuned_ratio, partials.base_pitch);
        ++harmonic_idx;
    }

    harmonic_idx = 1;
    for (int i = 2; i < kNumPartials; i += 3) {
        auto ratio = harmonic_idx + partials.ratios[i];
        auto detuned_ratio = ratio * ratio1;
        partials.freqs[i] = partials.base_frequency * detuned_ratio;
        partials.pitches[i] = RatioToPitch(detuned_ratio, partials.base_pitch);
        ++harmonic_idx;
    }
}

// =========================================================
// dissonance
// =========================================================
void Dissonance::Init(float sample_rate, float update_rate) {
}

void Dissonance::Process(Partials& partials) {
    if (!is_enable_) {
        for (int i = 0; i < kNumPartials; ++i) {
            auto ratio = i + 1.0f + partials.ratios[i];
            partials.freqs[i] = ratio * partials.base_frequency;
            partials.pitches[i] = RatioToPitch(ratio, partials.base_pitch);
        }
        return;
    }

    using enum param::DissonanceType::ParamEnum;
    switch (type_) {
    case kString:
        DoStringDiss(partials, string_stretch_factor_);
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
    default:
        break;
    }
}

void Dissonance::OnUpdateTick(const OscillorParams & params, int skip, int module_idx) {
    is_enable_ = params.dissonance.is_enable->GetBool();
    type_ = param::DissonanceType::GetEnum(params.dissonance.dissonance_type->GetInt());

    {
        auto raw_string = param::StringDissStretch::GetNumber(params.dissonance.args);
        auto ratio_idx = param::StringMultiRatio::GetChoiceIndex(params.dissonance.args);
        string_stretch_factor_ = raw_string * kStringRatios[ratio_idx];
    }

    harmonic_stretch_ratio_ = param::HarmonicStrech::GetNumber(params.dissonance.args);

    st_space_semitone_ = param::SemitoneSpace::GetNumber(params.dissonance.args);

    {
        error_ramp_ = param::ErrorRamp::GetNumber(params.dissonance.args);
        error_range_ = param::ErrorRange::GetNumber(params.dissonance.args);
    }

    {
        ratio2x_ratio_ = std::exp2(param::FakeUnisonRatio0::GetNumber(params.dissonance.args) / 12.0f);
        ratio3x_ratio_ = std::exp2(param::FakeUnisonRatio1::GetNumber(params.dissonance.args) / 12.0f);
    }

    {
        dispersion_amount_ = param::Dispersion_Amount::GetNumber(params.dissonance.args);
        dispersion_warp_ = param::Dispersion_Warp::GetNumber(params.dissonance.args);
    }
}

void Dissonance::OnNoteOn(int note) {
    for (auto& s : static_noise_) {
        auto v = static_cast<float>(random_()) / static_cast<float>(std::random_device::max());
        s = 2.0f * v - 1.0f;
    }
}

void Dissonance::OnNoteOff() {
}
}