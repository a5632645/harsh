#include "filter.h"

#include <numbers>
#include "param/param.h"
#include "param/filter_param.h"

namespace mana {
void Filter::Init(float sample_rate) {
    sample_rate_ = sample_rate;
}

void Filter::Process(Partials& partials) {
    final_cutoff_ = std::lerp(normalized_cutoff_, partials.base_frequency, key_track_);
    final_cutoff_semitone_ = std::lerp(cutoff_semitone_, partials.base_pitch, key_track_);

    switch (filter_type_) {
    case param::Filter_Type::TypeEnum::kLowpass:
        break;
    case param::Filter_Type::TypeEnum::kCombFilter:
        SetCombCutoff(std::lerp(normalized_cutoff_, partials.base_frequency * std::exp2(cutoff_semitone_ / 12.0f), key_track_),
                      cutoff_semitone_);
        DoCombFilter(partials);
        break;
    case param::Filter_Type::TypeEnum::kPhaser:
        DoPhaserFilter(partials);
        break;
    }
}

void Filter::OnUpdateTick(const SynthParam& params, int skip) {
    filter_type_ = param::IntChoiceParam<param::Filter_Type, param::Filter_Type::TypeEnum>::GetEnum(
        params.filter.filter_type
    );

    cutoff_semitone_ = param::FloatParam<param::Filter_Cutoff>::GetNumber(params.filter.arg0);
    normalized_cutoff_ = std::exp2(cutoff_semitone_ / 12.0f) * 8.1758f * 2.0f / sample_rate_;
    slope_ = param::FloatParam<param::Filter_Slope>::GetNumber(params.filter.arg2);
    key_track_ = param::FloatParam<param::Filter_KeyTracking>::GetNumber(params.filter.arg3);

    // comb
    comb_shape_ = param::FloatParam<param::Filter_CombShape>::GetNumber(params.filter.arg1);
    comb_phase_ = param::FloatParam<param::Filter_CombPhase>::GetNumber(params.filter.arg2) * std::numbers::pi_v<float> *2.0f;
    comb_depth_ = param::FloatParam<param::Filter_CombDepth>::GetNumber(params.filter.arg4);
    comb_phaser_ = param::FloatParam<param::Filter_CombPhaser>::GetNumber(params.filter.arg5);

    // phaser
    phaser_width_ = param::FloatParam<param::Filter_PhaserWidth>::GetNumber(params.filter.arg5);
    phaser_cycles_ = std::numbers::pi_v<float> / phaser_width_ * phaser_notches_;
    phaser_depth_ = param::FloatParam<param::Filter_PhaserDepth>::GetNumber(params.filter.arg4);
    phaser_notches_ = param::FloatParam<param::Filter_PhaserNotches>::GetNumber(params.filter.arg3);
    phaser_phase_ = param::FloatParam<param::Filter_PhaserPhase>::GetNumber(params.filter.arg2) * std::numbers::pi_v<float>*2.0f;
    phaser_shape_ = param::FloatParam<param::Filter_PhaserShape>::GetNumber(params.filter.arg1);
    phaser_begin_pitch_ = cutoff_semitone_ - phaser_width_;
    phaser_end_pitch_ = cutoff_semitone_ + phaser_width_;
}

void Filter::OnNoteOn(int note) {
}

void Filter::OnNoteOff() {
}

// ===============================================================
// comb filter
// ===============================================================
static float CombFunc(float x, float g) {
    auto t = 0.5f + 0.5f * std::cos(x);
    auto up = g * g - 2.0f * g + 1.0f;
    auto down = g * g * t - 2.0f * g * t + 1.0f;
    return up * t / down;
}

static float DecayMap(float x) {
    return 1.0f - std::exp(-50.0f * x);
}

void Filter::SetCombCutoff(float cutoff, float pitch) {
    comb_cycles_ = std::numbers::pi_v<float> *2.0f / cutoff;
    comb_phaser_cycles_ = std::numbers::pi_v<float> *2.0f / pitch;
}

void Filter::DoCombFilter(Partials& partials) {
    for (int i = 0; i < kNumPartials; ++i) {
        auto s = comb_shape_ * comb_shape_;
        auto comb_phase = comb_cycles_ * (partials.freqs[i] - key_track_ * partials.base_frequency) + comb_phase_;
        auto phaser_phase = comb_phaser_cycles_ * (partials.pitches[i] - key_track_ * partials.base_pitch) + comb_phase_;
        auto morph = s > 0.5f ? 1.0f : 2.0f * s;
        auto comb_val = CombFunc(comb_phase, comb_shape_ * 0.9f);
        auto phaser_val = CombFunc(phaser_phase, comb_shape_ * 0.9f);
        auto comb_val0 = CombFunc(comb_phase, 0.0f);
        auto phaser_val0 = CombFunc(phaser_phase, 0.0f);
        auto comb_f = std::lerp(comb_val0, comb_val, morph);
        auto phaser_f = std::lerp(phaser_val0, phaser_val, morph);
        auto gain = std::lerp(comb_val, phaser_val, comb_phaser_);
        //auto final_phase = std::lerp(comb_phase, phaser_phase, comb_phaser_);
        //auto gain_0 = CombFunc(final_phase, 0.0f);
        //auto gain_peak = CombFunc(final_phase, comb_shape_ * 0.9f);
        //auto gain = std::lerp(gain_0, gain_peak, morph);
        auto excite = 1.0f + 2.0f * s * s + 0.1f * comb_depth_;

        auto final_gain = std::lerp(1.0f, gain * excite, comb_depth_);

        partials.gains[i] *= final_gain;
    }
}

// ===============================================================
// phaser filter
// ===============================================================
void Filter::DoPhaserFilter(Partials& partials) {
    for (int i = 0; i < kNumPartials; ++i) {
        if (partials.pitches[i] >= phaser_begin_pitch_ && partials.pitches[i] <= phaser_end_pitch_) {
            auto s = phaser_shape_ * phaser_shape_;
            auto pitch_offset = partials.pitches[i] - cutoff_semitone_;
            auto phaser_phase = pitch_offset * phaser_cycles_ + phaser_phase_;
            auto morph = s > 0.5f ? 1.0f : 2.0f * s;
            auto phaser_val = CombFunc(phaser_phase, phaser_shape_ * 0.9f);
            auto phaser_val0 = CombFunc(phaser_phase, 0.0f);
            auto phaser_f = std::lerp(phaser_val0, phaser_val, morph);
            auto excite = 1.0f + std::lerp(0.0f, 2.0f * s * s, phaser_f);
            auto add_gain = std::lerp(0.1f * (1.0f - s), 0.0f, phaser_f);
            auto final_gain = std::lerp(1.0f, phaser_f * excite + add_gain, comb_depth_);
            //auto org_gain = 1.0f - PhaserFunc(pitch_offset * phaser_cycles_ + phaser_phase_, phaser_shape_);

            partials.gains[i] *= final_gain;
        }
    }
}
}