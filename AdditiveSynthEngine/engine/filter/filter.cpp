#include "filter.h"

#include <numbers>
#include "param/param.h"
#include "utli/convert.h"
#include "param/filter_param.h"
#include "engine/oscillor_param.h"

namespace mana {
void Filter::Init(float sample_rate, float update_rate) {
    sample_rate_ = sample_rate;
}

void Filter::Process(Partials& partials) {
    using enum param::Filter_Type::ParamEnum;

    switch (filter_type_) {
    case kLowpass:
        DoLowPassFilter(partials);
        break;
    case kHighpass:
        DoHighPassFilter(partials);
        break;
    case kBandpass:
        DoBandPassFilter(partials);
        break;
    case kBandstop:
        DoBandStopFilter(partials);
        break;
    case kCombFilter:
        SetCombCutoff(std::lerp(normalized_cutoff_, partials.base_frequency * std::exp2(cutoff_semitone_ / 12.0f), key_track_),
                      cutoff_semitone_);
        DoCombFilter(partials);
        break;
    case kPhaser:
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
    slope_ = param::Filter_Slope::GetNumber(filter_args_) / 12.0f; // make it db/oct
    key_track_ = param::Filter_KeyTracking::GetNumber(filter_args_);
    resonance_ = std::exp(0.11512925464970228420089957273422f * param::Filter_Resonance::GetNumber(filter_args_));
    resonance_width_ = param::Filter_BandWidth::GetNumber(filter_args_);
    cutoff_knee_ = param::Filter_Knee::GetNumber(filter_args_);
    filter_width_ = param::Filter_BandWidth::GetNumber(filter_args_);

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
    // parabola lowpass, idea from compressor
    float a1 = -slope_ / (4.0f * cutoff_knee_);
    float b1 = slope_ * (cutoff_semitone_ - cutoff_knee_) / (2.0f * cutoff_knee_);
    float c1 = -slope_ * (cutoff_semitone_ - cutoff_knee_) * (cutoff_semitone_ - cutoff_knee_) / (4.0f * cutoff_knee_);

    for (int i = 0; i < kNumPartials; ++i) {
        float output_db{};

        // filter section
        float knee_begin = cutoff_semitone_ - cutoff_knee_;
        float knee_end = cutoff_semitone_ + cutoff_knee_;
        float partial_p = partials.pitches[i];

        if (partial_p < knee_begin) {
            output_db = 0.0f;
        }
        else if (partial_p < knee_end) {
            output_db = a1 * partial_p * partial_p + b1 * partial_p + c1;
        }
        else {
            output_db = 0.0f - slope_ * (partial_p - cutoff_semitone_);
        }

        // resonance section

        // gain
        float gain = utli::DbToGain(output_db);
        partials.gains[i] *= gain;
    }
}

void Filter::DoHighPassFilter(Partials& partials) {
    // parabola highpass
    float a1 = -slope_ / (4.0f * cutoff_knee_);
    float b1 = slope_ * (cutoff_semitone_ + cutoff_knee_) / (2.0f * cutoff_knee_);
    float c1 = -slope_ * (cutoff_semitone_ + cutoff_knee_) * (cutoff_semitone_ + cutoff_knee_) / (4.0f * cutoff_knee_);

    for (int i = 0; i < kNumPartials; ++i) {
        float output_db{};

        // filter section
        float knee_begin = cutoff_semitone_ - cutoff_knee_;
        float knee_end = cutoff_semitone_ + cutoff_knee_;
        float partial_p = partials.pitches[i];

        if (partial_p < knee_begin) {
            output_db = 0.0f - slope_ * (cutoff_semitone_ - partial_p);
        }
        else if (partial_p < knee_end) {
            output_db = a1 * partial_p * partial_p + b1 * partial_p + c1;
        }
        else {
            output_db = 0.0f;
        }

        // resonance section

        // gain
        float gain = utli::DbToGain(output_db);
        partials.gains[i] *= gain;
    }
}

void Filter::DoBandPassFilter(Partials& partials) {
    // point
    auto lp_c = cutoff_semitone_ + filter_width_;
    auto hp_c = cutoff_semitone_ - filter_width_;
    auto hp_begin = hp_c - cutoff_knee_;
    auto hp_end = hp_c + cutoff_knee_;
    auto lp_begin = lp_c - cutoff_knee_;
    auto lp_end = lp_c + cutoff_knee_;

    // parabola lowpass
    float a1 = -slope_ / (4.0f * cutoff_knee_);
    float b1 = slope_ * (lp_c - cutoff_knee_) / (2.0f * cutoff_knee_);
    float c1 = -slope_ * (lp_c - cutoff_knee_) * (lp_c - cutoff_knee_) / (4.0f * cutoff_knee_);

    // parabola high pass
    float a2 = -slope_ / (4.0f * cutoff_knee_);
    float b2 = slope_ * (hp_c + cutoff_knee_) / (2.0f * cutoff_knee_);
    float c2 = -slope_ * (hp_c + cutoff_knee_) * (hp_c + cutoff_knee_) / (4.0f * cutoff_knee_);

    for (int i = 0; i < kNumPartials; ++i) {
        float partial_p = partials.pitches[i];
        float output_db{};

        if (partial_p < hp_begin) {
            output_db = 0.0f - slope_ * (hp_c - partial_p);
        }
        else if (partial_p < hp_end) {
            output_db = a2 * partial_p * partial_p + b2 * partial_p + c2;
        }
        else if(partial_p < lp_begin){
            output_db = 0.0f;
        }
        else if (partial_p < lp_end) {
            output_db = a1 * partial_p * partial_p + b1 * partial_p + c1;
        }
        else {
            output_db = 0.0f - slope_ * (partial_p - lp_c);
        }

        // gain
        float gain = utli::DbToGain(output_db);
        partials.gains[i] *= gain;
    }
}

void Filter::DoBandStopFilter(Partials& partials) {
    // point
    auto lp_c = cutoff_semitone_ - filter_width_;
    auto hp_c = cutoff_semitone_ + filter_width_;
    auto hp_begin = hp_c - cutoff_knee_;
    auto hp_end = hp_c + cutoff_knee_;
    auto lp_begin = lp_c - cutoff_knee_;
    auto lp_end = lp_c + cutoff_knee_;

    // parabola lowpass
    float a1 = -slope_ / (4.0f * cutoff_knee_);
    float b1 = slope_ * (lp_c - cutoff_knee_) / (2.0f * cutoff_knee_);
    float c1 = -slope_ * (lp_c - cutoff_knee_) * (lp_c - cutoff_knee_) / (4.0f * cutoff_knee_);

    // parabola high pass
    float a2 = -slope_ / (4.0f * cutoff_knee_);
    float b2 = slope_ * (hp_c + cutoff_knee_) / (2.0f * cutoff_knee_);
    float c2 = -slope_ * (hp_c + cutoff_knee_) * (hp_c + cutoff_knee_) / (4.0f * cutoff_knee_);

    for (int i = 0; i < kNumPartials; ++i) {
        float partial_p = partials.pitches[i];
        float output_db{};

        if (partial_p < lp_begin) {
            output_db = 0.0f;
        }
        else if (partial_p < lp_end) {
            output_db = a1 * partial_p * partial_p + b1 * partial_p + c1;
        }
        else if (partial_p < cutoff_semitone_) {
            output_db = 0.0f - slope_ * (partial_p - lp_c);
        }
        else if(partial_p < hp_begin){
            output_db = 0.0f - slope_ * (hp_c - partial_p);
        }
        else if(partial_p < hp_end){
            output_db = a2 * partial_p * partial_p + b2 * partial_p + c2;
        }
        else {
            output_db = 0.0f;
        }

        // gain
        float gain = utli::DbToGain(output_db);
        partials.gains[i] *= gain;
    }
}

void Filter::DoSmoothLowPassFilter(Partials& partials) {
    for (int i = 0; i < kNumPartials; ++i) {
        auto pitch = partials.pitches[i];
        auto reso_begin = cutoff_semitone_ - resonance_width_;
        auto reso_end = cutoff_semitone_ + resonance_width_;
        if (pitch < reso_begin) { // nothing
        }
        else if (pitch < cutoff_semitone_) {        // pass to reso
            auto db = std::lerp(0.0f, resonance_, 0.5f - 0.5f * std::cos(std::numbers::pi_v<float> *(pitch - reso_begin) / resonance_width_));
            auto gain = utli::DbToGain(db);
            partials.gains[i] *= gain;
        }
        else if (pitch < reso_end) {               // cutoff to reso
            auto db_at_reso = resonance_ - slope_ * resonance_width_ / 12.0f;
            auto db = std::lerp(db_at_reso, resonance_, 0.5f + 0.5f * std::cos(std::numbers::pi_v<float> *(pitch - cutoff_semitone_) / resonance_width_));
            auto gain = utli::DbToGain(db);
            partials.gains[i] *= gain;
        }
        else {                                     // cutoff to stop
            auto k = -slope_;
            auto db = resonance_ + k * (pitch - cutoff_semitone_) / 12.0f;
            auto gain = utli::DbToGain(db);
            partials.gains[i] *= gain;
        }
    }
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