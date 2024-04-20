#pragma once

#include <string_view>
#include <array>

namespace mana::param {
using namespace std::string_view_literals;

struct TimberType {
    enum class TimberEnum {
        kDualSaw = 0,
        kSync,
        kNumEnums
    };

    static constexpr auto kNames = std::to_array({
        "dual_saw"sv, "sync"sv
                                                 });
};
// =========================================================
// dual saw
// =========================================================
struct DualSaw_Ratio {
    static constexpr auto kName = "ratio"sv;
    static constexpr float kMin = 1.0f;
    static constexpr float kMax = 8.0f;
    static constexpr float kDefault = 1.0f;
    static constexpr int kTextPrecision = 0;
};

struct DualSaw_BeatingRate {
    static constexpr auto kName = "beating"sv;
    static constexpr float kMin = -8.0f;
    static constexpr float kMax = 8.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 3;
};

struct DualSaw_SawSquare {
    static constexpr auto kName = "saw_square"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 1.0f;
    static constexpr int kTextPrecision = 2;
};

// =========================================================
// sync
// =========================================================
struct Sync_WaveShape {
    enum class WaveShape {
        kSine = 0,
        kTriangle,
        kSquare,
        kSaw,
        kNumEnums
    };

    static constexpr auto kNames = std::to_array({
        "sine"sv,"tri"sv,"square"sv,"saw"sv
                                                 });

    static constexpr auto kName = "shape"sv;
};

struct Sync_Sync {
    static constexpr auto kName = "sync"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 12.0f * 5.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr auto kStuff = "st"sv;
    static constexpr int kTextPrecision = 1;
};
}