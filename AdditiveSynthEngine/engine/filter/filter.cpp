#include "filter.h"

#include <numbers>
#include "param/param.h"
#include "param/filter_param.h"
#include "engine/oscillor_param.h"

namespace mana {
void Filter::Init(float sample_rate, float update_rate) {
    sample_rate_ = sample_rate;
}

void Filter::Process(Partials& partials) {
    final_cutoff_ = std::lerp(normalized_cutoff_, partials.base_frequency, key_track_);
    final_cutoff_semitone_ = std::lerp(cutoff_semitone_, partials.base_pitch, key_track_);

    switch (filter_type_) {
    case param::Filter_Type::ParamEnum::kLowpass:
        DoLowPassFilter(partials);
        break;
    case param::Filter_Type::ParamEnum::kCombFilter:
        SetCombCutoff(std::lerp(normalized_cutoff_, partials.base_frequency * std::exp2(cutoff_semitone_ / 12.0f), key_track_),
                      cutoff_semitone_);
        DoCombFilter(partials);
        break;
    case param::Filter_Type::ParamEnum::kPhaser:
        DoPhaserFilter(partials);
        break;
    }
}

void Filter::PrepareParams(OscillorParams & params) {
    filter_type_arg_ = params.GetParam<IntParameter>("filter.type");
    for (int i = 0; auto & parg : filter_args_) {
        parg = params.GetPolyFloatParam("filter.arg{}", i++);
    }
}

void Filter::OnUpdateTick() {
    filter_type_ = param::Filter_Type::GetEnum(filter_type_arg_->GetInt());

    cutoff_semitone_ = param::Filter_Cutoff::GetNumber(filter_args_);
    normalized_cutoff_ = std::exp2(cutoff_semitone_ / 12.0f) * 8.1758f * 2.0f / sample_rate_;
    slope_ = param::Filter_Slope::GetNumber(filter_args_);
    key_track_ = param::Filter_KeyTracking::GetNumber(filter_args_);
    resonance_ = std::exp(0.11512925464970228420089957273422f * param::Filter_Resonance::GetNumber(filter_args_));

    // comb
    comb_shape_ = param::Filter_CombShape::GetNumber(filter_args_);
    comb_phase_ = param::Filter_CombPhase::GetNumber(filter_args_) * std::numbers::pi_v<float> *2.0f;
    comb_depth_ = param::Filter_CombDepth::GetNumber(filter_args_);
    comb_phaser_ = param::Filter_CombPhaser::GetNumber(filter_args_);

    // phaser
    phaser_width_ = param::Filter_PhaserWidth::GetNumber(filter_args_);
    phaser_cycles_ = std::numbers::pi_v<float> *2.0f / phaser_width_ * phaser_notches_;
    phaser_depth_ = param::Filter_PhaserDepth::GetNumber(filter_args_);
    phaser_notches_ = param::Filter_PhaserNotches::GetNumber(filter_args_);
    phaser_phase_ = param::Filter_PhaserPhase::GetNumber(filter_args_) * std::numbers::pi_v<float>*2.0f;
    phaser_shape_ = param::Filter_PhaserShape::GetNumber(filter_args_);
    phaser_begin_pitch_ = cutoff_semitone_ - phaser_width_ * 0.5f;
    phaser_end_pitch_ = cutoff_semitone_ + phaser_width_ * 0.5f;
}

void Filter::OnNoteOn(int note) {
}

void Filter::OnNoteOff() {
}

// ===============================================================
// low pass
// ===============================================================
void Filter::DoLowPassFilter(Partials& partials) {
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
            //auto s = phaser_shape_ * phaser_shape_;
            //auto pitch_offset = partials.pitches[i] - cutoff_semitone_;
            //auto phaser_phase = pitch_offset * phaser_cycles_ + phaser_phase_;
            //auto morph = s > 0.5f ? 1.0f : 2.0f * s;
            //auto phaser_val = CombFunc(phaser_phase, phaser_shape_ * 0.9f);
            //auto phaser_val0 = CombFunc(phaser_phase, 0.0f);
            //auto phaser_f = std::lerp(phaser_val0, phaser_val, morph);
            //auto excite = std::lerp(1.0f, 2.0f * s, s);
            //auto decay = std::lerp(1.0f, 0.8f * s, s);

            //auto phaser_total_width = phaser_end_pitch_ - phaser_begin_pitch_;
            //auto window_pos = pitch_offset / phaser_total_width * 2.0f * std::numbers::pi_v<float>;
            //auto up_window = excite * CombFunc(window_pos, -3.0f) + 1.0f;
            //auto down_window = 1.0f - decay * CombFunc(window_pos, -3.0f);
            //auto out_gain = std::lerp(down_window, up_window, phaser_f);
            //auto gain = std::lerp(1.0f, out_gain, phaser_depth_);

            //gain = up_window;
            //auto add_gain = std::lerp(0.1f * (1.0f - s), 0.0f, phaser_f);
            //auto final_gain = std::lerp(1.0f, phaser_f * excite + add_gain, comb_depth_);
            //auto org_gain = 1.0f - PhaserFunc(pitch_offset * phaser_cycles_ + phaser_phase_, phaser_shape_);

            auto pitch_offset = partials.pitches[i] - cutoff_semitone_;
            auto org_cos_val = -std::cos(pitch_offset * phaser_cycles_ + phaser_phase_);
            auto cos_shape_val = std::lerp(0.2f, 0.9f, phaser_shape_);
            auto shaped_cos_val = (org_cos_val - cos_shape_val) / (1.0f - org_cos_val * cos_shape_val);
            auto shaped_cos_val01 = 0.5f + 0.5f * shaped_cos_val;

            auto s = phaser_shape_ * phaser_shape_;
            auto up_a = 4.0f / phaser_width_ / phaser_width_;
            auto window_val = -up_a * pitch_offset * pitch_offset + 1.0f;
            auto up_excite = std::lerp(0.0f, 2.0f * s * s, s);
            auto up_window = 1.0f + window_val * up_excite;

            auto down_decay = std::lerp(1.0f, 0.99f, phaser_shape_);
            auto down_window = 1.0f - window_val;
            auto out_gain = std::lerp(down_window, up_window, shaped_cos_val01);

            auto gain = std::lerp(1.0f, out_gain, phaser_depth_);
            partials.gains[i] *= gain;
        }
    }
}
}