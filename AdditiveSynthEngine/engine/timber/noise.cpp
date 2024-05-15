#include "noise.h"

#include <ranges>
#include <algorithm>
#include "param/timber_param.h"
#include "utli/convert.h"

namespace mana {
constexpr std::array kDeltaPitch = []() {
    constexpr auto kFirstPitch = utli::cp::FreqToPitch(1.0f);
    std::array<float, kNumPartials> out{};
    for (int i = 0; i < kNumPartials; ++i) {
        out[i] = (utli::cp::FreqToPitch(1.0f + i) - kFirstPitch) / 12.0f;
    }
    return out;
}();

Noise::Noise() {
    random_.seed(0);
    std::ranges::generate(sync_noise_, [this]() {return urd_(random_); });
}

void Noise::Process(TimberFrame& frame) {
    auto db_comp = -color_ - 6.0f;
    if (db_comp < 0) {
        db_comp *= 4.0f;
    }

    for (int i = 0; i < kNumPartials; ++i) {
        auto noise_gain = utli::DbToGain(color_ * kDeltaPitch[i]);
        auto noise = std::lerp(sync_noise_[i], urd_(dynamic_random_), dynamic_amount_);
        auto noise_db = std::lerp(-60.0f, 0.0f, noise) + db_comp;

        frame.gains[i] = utli::DbToGain(noise_db) * noise_gain;
    }
}

void Noise::OnUpdateTick(OscParam& params) {
    dynamic_amount_ = param::Noise_Dynamic::GetNumber(params.args);
    color_ = param::Noise_Color::GetNumber(params.args);

    int new_seed = static_cast<int>(std::round(param::Noise_Seed::GetNumber(params.args)));
    if (new_seed != seed_) {
        seed_ = new_seed;
        random_.seed(new_seed);
        std::ranges::generate(sync_noise_, [this]() {return urd_(random_); });
    }
}
}