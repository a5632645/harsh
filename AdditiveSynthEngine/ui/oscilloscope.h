#pragma once

#include <vector>
#include "engine/synth.h"

namespace mana {
class Oscilloscope {
public:
    Oscilloscope(const Synth& synth);

    void Paint();
    void SetBounds(int x, int y, int w, int h);
    void PushBuffer(const std::vector<float>& data);
private:
    bool PushData(float s);

    const Synth& synth_;

    std::vector<float> data_buffer_;
    std::vector<float> data_buffer2_;
    int write_pos_{};
    float last_sample_{};
    int counter_{};
    int nsample_per_pixel_{ 4 };

    int x_{};
    int w_{};
    int y_center_{};
    int y_scale_{};
};
}