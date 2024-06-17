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
        std::array<float, kNumPartials> db_gains{};
        std::array<float, kNumPartials> ratio_diffs{}; // name to series bin plus this to a final ratio
    };

    void DuplicateExtraDataForLerp() {
        const auto last_frame = frames.back();
        frames.emplace_back(last_frame);
        frames.emplace_back(last_frame);
    }

    std::vector<FftFrame> frames;
    int num_frame{};
    float frame_interval_sample{};
    float base_freq{};
    float level_up_db{};
    Type source_type{ Type::kUnknow };
};
}