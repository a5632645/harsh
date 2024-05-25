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
        using ut = param::Unison_Type::ParamEnum;
        auto type = param::Unison_Type::GetEnum(unison_type_->GetInt());

        if (type == ut::kHzUniform) {
            auto max_freq_diff = (1.0f - std::exp2(-pitch_->GetValue() / 12.0f)) * partials.base_frequency;
            for (int i = 0; i < 9; ++i) {
                auto p_inc = (max_freq_diff * voice_ratios_[i]) * update_skip_;
                voice_phases_[i] += p_inc;
                voice_phases_[i] -= static_cast<int>(voice_phases_[i]);
            }
        }
        else {
            for (int i = 0; i < 9; ++i) {
                auto freq_ratio = std::exp2(voice_ratios_[i] * pitch_->GetValue() / 12.0f);
                auto voice_base_freq = partials.base_frequency * freq_ratio;
                auto p_inc = (voice_base_freq - partials.base_frequency) * update_skip_;
                voice_phases_[i] += p_inc;
                voice_phases_[i] -= static_cast<int>(voice_phases_[i]);
            }
        }

        float avg_gain = 1.0f;
        //float avg_gain = 1.0f / static_cast<float>((num_voice + 1.0f) / 2.0f);
        for (int i = 0; i < kNumPartials; ++i) {
            float gain = 0.0f;
            for (int j = 0; j < num_voice; ++j) {
                gain += std::cos(std::numbers::pi_v<float> *2.0f * voice_phases_[j] * partials.ratios[i]);
            }
            partials.gains[i] *= gain * avg_gain;
        }
    }
}

void Unison::OnUpdateTick() {
    float pitch_scale = pitch_->GetValue();
    int num_voice = num_voice_->GetInt();

    if (old_num_voice_ != num_voice) {
        old_num_voice_ = num_voice;

        using ut = param::Unison_Type::ParamEnum;
        auto type = param::Unison_Type::GetEnum(unison_type_->GetInt());
        if (type != ut::kRandom) {
            voice_ratios_ = kUniformTable[num_voice - 1];
        }
    }

    //for (int i = 0; i < num_voice; ++i) {
    //    float pitch_shift = static_cast<float>(i) * pitch_scale / static_cast<float>(num_voice);
    //    voice_ratios_[i] = std::exp2(pitch_shift / 12.0f);
    //}
    //for (int i = num_voice; i < 9; ++i) {
    //    voice_ratios_[i] = 1.0f;
    //}
}

void Unison::OnNoteOn() {
    std::ranges::generate(voice_phases_, [this]() {
        return urd_(random_) * phase_->GetValue();
    });

    using enum param::Unison_Type::ParamEnum;
    auto type = param::Unison_Type::GetEnum(unison_type_->GetInt());
    switch (type) {
    case kRandom:
        std::ranges::generate(voice_ratios_, [this]() {return urd_(random_); });
        break;
    default:
        break;
    }
}

void Unison::OnNoteOff() {
}
}