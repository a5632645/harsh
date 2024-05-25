#pragma once

#include "timber_frame.h"
#include "osc_param.h"
#include "param/param_helper.h"
#include "param/timber_param.h"
#include <numbers>
#include <ranges>

namespace mana {
class Impulse {
public:
    void Init(float sample_rate, float update_rate) {}
    void Process(TimberFrame& frame) {
        using namespace std::ranges::views;

        int num_sine = static_cast<int>(std::round(num_sines_));
        auto gain = 1.0f / num_sines_;

        std::ranges::fill(frame.gains | take(num_sine), gain);
        std::ranges::fill(frame.gains | drop(num_sine), 0.0f);
    }

    void OnUpdateTick(OscParam& params) {
        num_sines_ = helper::GetAlterParamValue(params.args, param::Impulse_NumSines{}) * kNumPartials;
        num_sines_ = std::clamp<float>(num_sines_, 1.0f, kNumPartials);
    }

    void OnNoteOn(int note) {}
    void OnNoteOff() {}
private:
    float num_sines_{};
};
}