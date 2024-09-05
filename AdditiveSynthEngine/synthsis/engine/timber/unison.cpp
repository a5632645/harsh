#include "unison.h"

#include <numbers>
#include <cmath>
#include <array>
#include "param/unison_param.h"
#include "utli/convert.h"

namespace mana::detail {
static consteval auto MakeUniformArray(int size) {
    std::array<float, 9> table{};
    if (size == 1) {
        table[0] = 1.0f;
    }
    else {
        auto interval = 2.0f / (size - 1.0f);
        table[0] = -1.0f;
        for (int i = 1; i < size; ++i) {
            table[i] = table[i - 1] + interval;
        }
    }
    return table;
}
static consteval auto MakeUniformTable(int size) {
    std::array<std::array<float, 9>, 9> table{};
    for (int i = 1; i <= 9; ++i) {
        table[i - 1] = MakeUniformArray(i);
    }
    return table;
}
}

namespace mana {
static constexpr auto kUniformTable = detail::MakeUniformTable(9);
}

namespace mana {
void Unison::Init(float sample_rate, float update_rate) {
    inv_update_rate_ = 1.0f / update_rate;
}

void Unison::PrepareParams(ModulableParams& params) {
    arg_num_voice_ = params.GetParam<IntParameter>("unison.num_voice");
    pitch_ = params.GetModuFloatParam("unison.pitch");
    phase_ = params.GetModuFloatParam("unison.phase");
    pan_ = params.GetModuFloatParam("unison.pan");
    morph_ = params.GetModuFloatParam("unison.morph");
    randomness_ = params.GetModuFloatParam("unison.randomness");
}

void Unison::Process(Partials& partials) {
    if (num_voice_ == 1)
        return;

    auto randomness = randomness_->GetValue();
    const auto& uniform_lut = kUniformTable[num_voice_ - 1];
    for (int i = 0; i < num_voice_; ++i) {
        voice_ratios_[i] = std::lerp(uniform_lut[i], random_voice_ratios_[i], randomness);
    }

    auto max_freq_diff = (std::exp2(pitch_->GetValue() / 12.0f) - 1.0f) * partials.base_frequency * inv_update_rate_;
    for (int i = 0; i < num_voice_; ++i) {
        auto freq_diff = max_freq_diff * voice_ratios_[i];
        voice_phases_[i] += freq_diff;
        voice_phases_[i] -= static_cast<int>(voice_phases_[i]);
    }

    for (int i = 0; i < kNumPartials; ++i) {
        auto freq_diff = max_freq_diff * partials.ratios[i];
        rand_phase_[i] += freq_diff;
        if (rand_phase_[i] > 1.0f) {
            last_rand_[i] = curr_rand_[i];
            curr_rand_[i] = urd_(random_);
        }
        rand_phase_[i] -= static_cast<int>(rand_phase_[i]);
    }

    auto morph = morph_->GetValue();
    for (int i = 0; i < kNumPartials; ++i) {
        float gain = 0.0f;
        for (int j = 0; j < num_voice_; ++j) {
            auto wave_val = std::cos(std::numbers::pi_v<float> *2.0f * voice_phases_[j] * partials.ratios[i]);
            auto noise_val = std::lerp(last_rand_[i], curr_rand_[i], rand_phase_[i]);
            gain += std::lerp(wave_val, noise_val, morph);
        }
        partials.gains[i] *= gain;
    }
}

void Unison::OnUpdateTick() {
    num_voice_ = arg_num_voice_->GetInt();
}

void Unison::OnNoteOn() {
    std::ranges::generate(voice_phases_, [this]() {
        return urd_(random_) * phase_->GetValue();
    });

    std::ranges::generate(random_voice_ratios_, [this]() {return urd_(random_); });
}

void Unison::OnNoteOff() {
}
}