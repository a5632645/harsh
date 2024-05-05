#pragma once

#include <array>
#include <vector>
#include "engine/EngineConfig.h"

namespace mana {
struct ResynthsisFrames {
    struct FftFrame {
        std::array<float, kFFtSize / 2> gains;
        std::array<float, kFFtSize / 2> freq_diffs;
    };

    std::vector<FftFrame> frames;
    float data_sample_rate;
    float data_series_freq;
    float max_fre_diff;
    float frame_interval_sample;
};
}