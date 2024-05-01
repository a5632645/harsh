#include "dissonance.h"

namespace mana {
static void DoHarmonicStretch(Partials& partials, float stretch) {
    auto ratio = 1.0f;
    auto base_freq = partials.base_frequency;

    for (int i = 0; i < kNumPartials; ++i) {
        auto final_ratio = ratio + partials.freqs[i];
        partials.freqs[i] = base_freq * final_ratio;
        partials.pitches[i] = partials.base_pitch + 12.0f * std::log2(final_ratio);
        ratio += stretch;
    }
}

static void DoSemitoneSpace(Partials& partials, float semitone) {
    auto ratio = 1.0f;
    auto ratio_mul = std::exp2(semitone / 12.0f);
    auto base_freq = partials.base_frequency;

    for (int i = 0; i < kNumPartials; ++i) {
        auto final_ratio = ratio + partials.freqs[i];
        partials.freqs[i] = final_ratio * base_freq;
        partials.pitches[i] = partials.base_pitch + 12.0f * std::log2(final_ratio);
        ratio *= ratio_mul;
    }
}

static constexpr std::array kStringRatios{ 1.0f, 0.1f,0.01f,0.001f,0.0001f };
static void DoStringDiss(Partials& partials, float string_val) {
    for (int i = 0; i < kNumPartials; ++i) {
        auto n = i + 1.0f + partials.freqs[i];
        auto ratio = n * std::sqrt(1.0f + n * n * string_val);
        partials.freqs[i] = partials.base_frequency * ratio;
        partials.pitches[i] = partials.base_pitch + 12.0f * std::log2(ratio);
    }
}

// =========================================================
// dissonance
// =========================================================
void Dissonance::Init(float sample_rate) {
}

void Dissonance::Process(Partials& partials) {
    if (!is_enable_) {
        for (int i = 0; i < kNumPartials; ++i) {
            auto ratio = i + 1.0f + partials.freqs[i];
            partials.freqs[i] = ratio * partials.base_frequency;
            partials.pitches[i] = partials.base_pitch + 12.0f * std::log2(ratio);
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
    default:
        break;
    }
}

void Dissonance::OnUpdateTick(const SynthParam& params, int skip, int module_idx) {
    is_enable_ = params.dissonance.is_enable;
    type_ = param::DissonanceType::GetEnum(params.dissonance.dissonance_type);

    {
        auto raw_string = param::StringDissStretch::GetNumber(params.dissonance.args);
        auto ratio_idx = param::StringMultiRatio::GetChoiceIndex(params.dissonance.args);
        string_stretch_factor_ = raw_string * kStringRatios[ratio_idx];
    }

    harmonic_stretch_ratio_ = param::HarmonicStrech::GetNumber(params.dissonance.args);

    st_space_semitone_ = param::SemitoneSpace::GetNumber(params.dissonance.args);
}

void Dissonance::OnNoteOn(int note) {
}

void Dissonance::OnNoteOff() {
}
}