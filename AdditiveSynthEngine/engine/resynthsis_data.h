#pragma once

#include <array>
#include <vector>
#include "engine/EngineConfig.h"

namespace mana {
struct ResynthsisFrames {
    struct FftFrame {
        std::array<float, kNumPartials> gains;
        std::array<float, kNumPartials> freq_diffs;
    };

    std::vector<FftFrame> frames;
    float data_sample_rate;
    float data_series_freq;
};
}