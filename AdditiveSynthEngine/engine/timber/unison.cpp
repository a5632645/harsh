#include "unison.h"

#include <numbers>
#include <cmath>
#include "param/timber_param.h"
#include "utli/convert.h"

namespace mana {
void Unison::Init(float sample_rate, float update_rate) {
    update_skip_ = static_cast<int>(std::round(sample_rate / update_rate));
}

void Unison::PrepareParams(OscillorParams& params) {
    unison_type_ = params.GetParam<IntParameter>("unison.type");
    num_voice_ = params.GetParam<IntParameter>("unison.num_voice");
    pitch_ = params.GetPolyFloatParam("unison.pitch");
    phase_ = params.GetPolyFloatParam("unison.phase");
    pan_ = params.GetPolyFloatParam("unison.pan");
}

void Unison::Process(Partials& partials) {
    int num_voice = num_voice_->GetInt() + 1;
    if (num_voice != 1) {
        float avg_gain = 1.0f / static_cast<float>((num_voice + 1.0f) / 2.0f);
        for (int i = 0; i < kNumPartials; ++i) {
            float gain = 0.0f;
            for (int j = 0; j < num_voice; ++j) {
                float voice_base_fre = partials.base_frequency * voice_ratios_[j];
                gain += std::cos(std::numbers::pi_v<float> *(voice_base_fre * partials.ratios[i] - partials.freqs[i]) * time_);
            }
            partials.gains[i] *= gain * avg_gain;
        }
    }

    time_ += update_skip_;
}

void Unison::OnUpdateTick() {
    float pitch_scale = param::Unison_Pitch::GetNumber(pitch_->GetValue());
    int num_voice = num_voice_->GetInt() + 1;

    for (int i = 0; i < num_voice; ++i) {
        float pitch_shift = static_cast<float>(i) * pitch_scale / static_cast<float>(num_voice);
        voice_ratios_[i] = std::exp2(pitch_shift / 12.0f);
    }
    for (int i = num_voice; i < 9; ++i) {
        voice_ratios_[i] = 1.0f;
    }
}

void Unison::OnNoteOn() {
    time_ = 0;
}

void Unison::OnNoteOff() {
}
}