#pragma once

#include <array>
#include "param/param.h"

namespace mana::param {
struct PitchBend {
    static constexpr auto kName = "pitch_bend"sv;
    static constexpr float kMin = -60.0f;
    static constexpr float kMax = 60.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr std::string_view kStuff = "st"sv;
    static constexpr int kTextPrecision = 1;
};

struct PhaseType {
    enum class PhaseTypeEnum {
        kRandom = 0,
        kDispersion,
        kNumEnums
    };

    static constexpr auto kName = "type"sv;
    static constexpr auto kNames = std::to_array({
            "random"sv,"dispersion"sv });
};

struct PhaseRandomAmount {
    static constexpr auto kName = "amount"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 2;
};

struct PhaseDispersionAmount {
    static constexpr auto kName = PhaseRandomAmount::kName;
    static constexpr float kMin = -1.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 2;
};
}