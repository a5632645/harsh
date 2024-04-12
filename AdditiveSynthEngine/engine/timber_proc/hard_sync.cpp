#include "hard_sync.h"

#include <cmath>
#include <numbers>

static float Sinc(float x) {
    if (x == 0.0f) {
        return 1.0f;
    }
    return std::sin(std::numbers::pi_v<float> *x) / (std::numbers::pi_v<float> *x);
}   

namespace mana {
HardSync::HardSync(const param::HardSyncParam & p)
    : param_(p) {
}

void HardSync::Init(float sample_rate) {
}

void HardSync::Process(Partials& partials) {
    if (param_.sync == 1.0f) {
        return;
    }

    std::ranges::copy(partials.gains, saved_gains_.begin());
    for (int partial_idx = 0; partial_idx < kNumPartials; ++partial_idx) {
        auto gain = 0.0f;

        for (int i = 1 - kNumPartials; i < kNumPartials; ++i) {
            auto idx = partial_idx + i;
            if (idx < 0 || idx >= kNumPartials) {
                continue;
            }

            gain += saved_gains_[idx] * Sinc((partial_idx + 1) - (idx + 1) * param_.sync);
        }
        partials.gains[partial_idx] = gain;
    }
}

void HardSync::OnUpdateTick(int skip) {
}

void HardSync::OnNoteOn(int note) {
}

void HardSync::OnNoteOff() {
}
}