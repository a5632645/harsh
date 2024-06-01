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
    arg_num_voice_ = params.GetParam<IntParameter>("unison.num_voice");
    pitch_ = params.GetPolyFloatParam("unison.pitch");
    phase_ = params.GetPolyFloatParam("unison.phase");
    pan_ = params.GetPolyFloatParam("unison.pan");
}

void Unison::Process(Partials& partials) {
    if (num_voice_ == 1)
        return;

    using ut = param::Unison_Type::ParamEnum;
    auto type = param::Unison_Type::GetEnum(unison_type_->GetInt());
    switch (type) {
    case ut::kHzUniform:
        HzUniformProcess(partials);
        break;
    case ut::kPUniform:
        UniformProcess(partials);
        break;
    case ut::kRandom:
        RandomProcess(partials);
        break;
    default:
        assert(false);
        break;
    }
}

static constexpr auto kHzUniformTable = []() {
    std::array<std::array<float, 4>, 10> table{};
    for (int i = 2; i < table.size(); ++i) {// idx: 2..9
        int num_cos = i / 2;
        float interval = 1.0f / num_cos;
        for (int j = 0; j < num_cos; ++j) {
            table[i][j] = interval * (j + 1.0f);
        }
    }
    return table;
}();
void Unison::HzUniformProcess(Partials& partials) {
    float center_gain = num_voice_ % 2 == 1 ? 1.0f : 0.0f;
    int num_rm = num_voice_ / 2;
    auto max_freq_diff = (std::exp2(pitch_->GetValue() / 12.0f) - 1.0f) * partials.base_frequency;
    const auto& uniform_table = kHzUniformTable[num_voice_];

    for (int osc_idx = 0; osc_idx < num_rm; ++osc_idx) {
        float rm_rate = uniform_table[osc_idx] * max_freq_diff;
        auto p_inc = rm_rate * update_skip_;
        voice_phases_[osc_idx] += p_inc;
        voice_phases_[osc_idx] -= static_cast<int>(voice_phases_[osc_idx]);
    }

    for (int har_idx = 0; har_idx < kNumPartials; ++har_idx) {
        float gain = center_gain;
        for (int osc_idx = 0; osc_idx < num_rm; ++osc_idx) {
            gain += std::cos(std::numbers::pi_v<float> *2.0f * voice_phases_[osc_idx] * partials.ratios[har_idx]);
        }
        partials.gains[har_idx] *= gain;
    }
}

void Unison::UniformProcess(Partials& partials) {
    const auto& voice_ratios = kUniformTable[num_voice_ - 1];

    auto max_freq_diff = (std::exp2(pitch_->GetValue() / 12.0f) - 1.0f) * partials.base_frequency;
    for (int i = 0; i < num_voice_; ++i) {
        auto freq_diff = max_freq_diff * voice_ratios[i];
        auto p_inc = freq_diff * update_skip_;
        voice_phases_[i] += p_inc;
        voice_phases_[i] -= static_cast<int>(voice_phases_[i]);
    }

    for (int i = 0; i < kNumPartials; ++i) {
        float gain = 0.0f;
        for (int j = 0; j < num_voice_; ++j) {
            gain += std::cos(std::numbers::pi_v<float> *2.0f * voice_phases_[j] * partials.ratios[i]);
        }
        partials.gains[i] *= gain;
    }
}

void Unison::RandomProcess(Partials& partials) {
    for (int i = 0; i < num_voice_; ++i) {
        auto freq_ratio = std::exp2(random_voice_ratios_[i] * pitch_->GetValue() / 12.0f);
        auto voice_base_freq = partials.base_frequency * freq_ratio;
        auto p_inc = (voice_base_freq - partials.base_frequency) * update_skip_;
        voice_phases_[i] += p_inc;
        voice_phases_[i] -= static_cast<int>(voice_phases_[i]);
    }

    for (int i = 0; i < kNumPartials; ++i) {
        float gain = 1.0f;
        for (int j = 0; j < num_voice_; ++j) {
            gain += std::cos(std::numbers::pi_v<float> *2.0f * voice_phases_[j] * partials.ratios[i]);
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