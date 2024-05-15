#pragma once

#include <random>
#include <array>
#include "engine/EngineConfig.h"
#include "timber_frame.h"
#include "osc_param.h"

namespace mana {
class Noise {
public:
    Noise();

    void Init(float sample_rate, float update_rate) {}
    void Process(TimberFrame& frame);
    void OnUpdateTick(OscParam& params);
    void OnNoteOn(int note) {}
    void OnNoteOff() {}
private:
    std::array<float, kNumPartials> sync_noise_;
    std::default_random_engine random_;
    std::default_random_engine dynamic_random_;
    std::uniform_real_distribution<float> urd_;
    
    float dynamic_amount_{};
    float color_{};
    int seed_{ -1 };
};
}