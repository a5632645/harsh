#include "unison.h"

#include <numbers>
#include <cmath>
#include "param/unison_param.h"
#include "utli/convert.h"

namespace mana {
void Unison::Init(float sample_rate, float update_rate) {
    update_skip_ = sample_rate / update_rate;
}

void Unison::PrepareParams(OscillorParams& params) {
    unison_type_ = params.GetParam<IntChoiceParameter>("unison.type");
    num_voice_ = params.GetParam<IntParameter>("unison.num_voice");
    pitch_ = params.GetPolyFloatParam("unison.pitch");
    phase_ = params.GetPolyFloatParam("unison.phase");
    pan_ = params.GetPolyFloatParam("unison.pan");
}

void Unison::Process(Partials& partials) {
    int num_voice = num_voice_->GetInt();
    if (num_voice != 1) {
        for (int i = 0; i < 9; ++i) {
            auto voice_base_freq = partials.base_frequency * voice_ratios_[i];
            auto p_inc = (voice_base_freq - partials.base_frequency) * update_skip_;
            voice_phases_[i] += p_inc;
            voice_phases_[i] -= static_cast<int>(voice_phases_[i]);
        }

        float avg_gain = 1.0f;
        //float avg_gain = 1.0f / static_cast<float>((num_voice + 1.0f) / 2.0f);
        for (int i = 0; i < kNumPartials; ++i) {
            float gain = 0.0f;
            for (int j = 0; j < num_voice; ++j) {
                float voice_base_fre = partials.base_frequency * voice_ratios_[j];
                gain += std::cos(std::numbers::pi_v<float> *2.0f * voice_phases_[j] * partials.ratios[i]);
            }
            partials.gains[i] *= gain * avg_gain;
        }
    }
}

void Unison::OnUpdateTick() {
    float pitch_scale = pitch_->GetValue();
    int num_voice = num_voice_->GetInt();

    for (int i = 0; i < num_voice; ++i) {
        float pitch_shift = static_cast<float>(i) * pitch_scale / static_cast<float>(num_voice);
        voice_ratios_[i] = std::exp2(pitch_shift / 12.0f);
    }
    for (int i = num_voice; i < 9; ++i) {
        voice_ratios_[i] = 1.0f;
    }
}

void Unison::OnNoteOn() {
    voice_phases_.fill(0.0f);
}

void Unison::OnNoteOff() {
}
}