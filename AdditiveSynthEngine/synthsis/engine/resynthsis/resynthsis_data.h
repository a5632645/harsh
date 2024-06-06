#pragma once

#include <array>
#include <vector>
#include "engine/EngineConfig.h"

namespace mana {
struct ResynthsisFrames {
    enum class Type {
        kUnknow,
        kAudio,
        kImage
    };

    struct FftFrame {
        std::array<float, kNumPartials> gains{};
        std::array<float, kNumPartials> ratio_diffs{}; // name to series bin plus this to a final ratio
    };

    std::vector<FftFrame> frames;
    float frame_interval_sample{};
    float base_freq{};
    Type source_type{ Type::kUnknow };
};
}