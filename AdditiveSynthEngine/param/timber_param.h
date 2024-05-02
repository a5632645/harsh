#pragma once

#include <string_view>
#include <array>
#include "param.h"

namespace mana::param {
using namespace std::string_view_literals;

struct TimberType : IntChoiceParam<TimberType> {
    enum class ParamEnum {
        kDualSaw = 0,
        kSync,
        kNumEnums
    };

    static constexpr std::array kNames{
        "dual_saw"sv,
        "sync"sv
    };
};
// =========================================================
// dual saw
// =========================================================
struct DualSaw_Ratio : FloatParam<DualSaw_Ratio> {
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "ratio"sv;
    static constexpr float kMin = 1.0f;
    static constexpr float kMax = 8.0f;
    static constexpr float kDefault = 1.0f;
    static constexpr int kTextPrecision = 0;
};

struct DualSaw_BeatingRate : FloatParam<DualSaw_BeatingRate> {
    static constexpr int kArgIdx = 1;
    static constexpr auto kName = "beating"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 10.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr int kTextPrecision = 3;
    static constexpr auto kStuff = "hz"sv;
};

struct DualSaw_SawSquare : FloatParam<DualSaw_SawSquare> {
    static constexpr int kArgIdx = 2;
    static constexpr auto kName = "saw_square"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 1.0f;
    static constexpr float kDefault = 1.0f;
    static constexpr int kTextPrecision = 2;
};

// =========================================================
// sync
// =========================================================
struct Sync_WaveShape : FloatChoiceParam<Sync_WaveShape> {
    enum class ParamEnum {
        kSine = 0,
        kTriangle,
        kSquare,
        kSaw,
        kNumEnums
    };

    static constexpr std::array kNames{
        "sine"sv,"tri"sv,"square"sv,"saw"sv
    };
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "shape"sv;
};

struct Sync_Sync : FloatParam<Sync_Sync> {
    static constexpr int kArgIdx = 1;
    static constexpr auto kName = "sync"sv;
    static constexpr float kMin = 0.0f;
    static constexpr float kMax = 12.0f * 5.0f;
    static constexpr float kDefault = 0.0f;
    static constexpr auto kStuff = "st"sv;
    static constexpr int kTextPrecision = 1;
};
}