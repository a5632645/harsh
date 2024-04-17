#pragma once

#include <array>
#include "param/param.h"

namespace mana::param {
struct DissonanceType {
    enum class DissonaceTypeEnum {
        kString = 0,
        kHarmonicStretch,
        kSemitoneSpace,
        kNumEnums
    };

    static constexpr auto kName = "type"sv;
    static constexpr auto kNames = std::to_array({
            "string"sv,"harmonic"sv,"st space"sv
                                                 });
};

// harmonic stretch
struct HarmonicStrech {
    static constexpr auto kName = "stretch"sv;
    static constexpr auto kMin = 1.0f;
    static constexpr auto kMax = 4.0f;
    static constexpr auto kDefault = 1.0f;
    static constexpr auto kTextPrecision = 3;
};

// =========================================================
// semitone space
// =========================================================
struct SemitoneSpace {
static constexpr auto kName = "pitch"sv;
static constexpr auto kMin = 0.0f;
static constexpr auto kMax = 24.0f;
static constexpr auto kDefault = 1.0f;
static constexpr auto kTextPrecision = 1;
};

// =========================================================
// string
// =========================================================
struct StringDissStretch {
    static constexpr auto kName = "string"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 0.1f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 5;
};

}