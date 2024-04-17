#include "dissonance.h"

namespace mana {
static void DoHarmonicStretch(Partials& partials, float stretch) {
    auto ratio = 1.0f;
    auto base_freq = partials.base_frequency;

    for (int i = 0; i < kNumPartials; ++i) {
        partials.freqs[i] = base_freq * ratio;
        ratio += stretch;
    }
}

static void DoSemitoneSpace(Partials& partials, float semitone) {
    auto ratio = 1.0f;
    auto ratio_mul = std::exp2(semitone / 12.0f);
    auto base_freq = partials.base_frequency;

    for (int i = 0; i < kNumPartials; ++i) {
        partials.freqs[i] = ratio * base_freq;
        ratio *= ratio_mul;
    }
}

void Dissonance::Init(float sample_rate) {
}

void Dissonance::Process(Partials& partials) {
    if (!is_enable_) {
        for (int i = 0; i < kNumPartials; ++i) {
            partials.freqs[i] = (i + 1.0f) * partials.base_frequency;
        }
        return;
    }

    using enum param::DissonanceType::DissonaceTypeEnum;
    switch (type_) {
    case kString:
        string_.Process(partials);
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

void Dissonance::OnUpdateTick(const SynthParam& params, int skip) {
    is_enable_ = params.dissonance.is_enable;
    type_ = param::IntChoiceParam<param::DissonanceType, param::DissonanceType::DissonaceTypeEnum>::GetEnum(
        params.dissonance.dissonance_type
    );

    auto arg0 = params.dissonance.arg0;
    auto arg1 = params.dissonance.arg1;

    using enum param::DissonanceType::DissonaceTypeEnum;
    switch (type_) {
    case kString:
        string_.OnUpdateTick(params, skip);
        break;
    case kHarmonicStretch:
        harmonic_stretch_ratio_ = param::FloatParam<param::HarmonicStrech>::GetNumber(arg0);
        break;
    case kSemitoneSpace:
        st_space_semitone_ = param::FloatParam<param::SemitoneSpace>::GetNumber(arg0);
    default:
        break;
    }
}

void Dissonance::OnNoteOn(int note) {
}

void Dissonance::OnNoteOff() {
}
}