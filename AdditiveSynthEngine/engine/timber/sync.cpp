#include "sync.h"

#include <numbers>
#include "param/param.h"
#include "param/timber.h"
#include "engine/VolumeTable.hpp"

namespace mana {
static constexpr auto kSineTable = makeHarmonicArray<kNumPartials>([](float x) {return x == 1.0f ? 1.0f : 0.0f; });
static constexpr auto kTriTable = makeHarmonicArray<kNumPartials>([](float x) {
    auto xint = static_cast<int>(x);
    if ((xint & 1) == 0) {
        return 0.0f;
    }
    
    auto x2 = (xint - 1) / 2;
    auto sign = 1.0f;
    if ((x2 & 1) == 1) {
        sign = -1.0f;
    }
    auto pi2 = std::numbers::pi_v<float> *std::numbers::pi_v<float>;
    return 8.0f * sign / pi2 / xint / xint;
});
static constexpr auto kSawTable = VolumeTable<kNumPartials>::SAW_TABLE;
static constexpr auto kSquareTable = VolumeTable<kNumPartials>::PULSE_TABLE;

static constexpr float GetPartialGain(param::Sync_WaveShape::WaveShape a, int idx) {
    switch (a) {
    case mana::param::Sync_WaveShape::WaveShape::kSine:
        return kSineTable[idx];
    case mana::param::Sync_WaveShape::WaveShape::kTriangle:
        return kTriTable[idx];
    case mana::param::Sync_WaveShape::WaveShape::kSaw:
        return kSawTable[idx];
    case mana::param::Sync_WaveShape::WaveShape::kSquare:
        return kSquareTable[idx];
    }
}

static constexpr float GetPartialGainFrac(param::Sync_WaveShape::WaveShape a,
                                          param::Sync_WaveShape::WaveShape b,
                                          float frac,
                                          int idx) {
    auto a_gain = GetPartialGain(a, idx);
    auto b_gain = GetPartialGain(b, idx);
    return std::lerp(a_gain, b_gain, frac);
}

static float Sinc(float x) {
    if (x == 0.0f) {
        return 1.0f;
    }
    return std::sin(std::numbers::pi_v<float> *x) / (std::numbers::pi_v<float> *x);
}

void Sync::Init(float sample_rate) {
}

void Sync::Process(Partials& partials) {
    for (int partial_idx = 0; partial_idx < kNumPartials; ++partial_idx) {
        auto gain = 0.0f;

        for (int i = 1 - kNumPartials; i < kNumPartials; ++i) {
            auto idx = partial_idx + i;
            if (idx < 0 || idx >= kNumPartials) {
                continue;
            }

            gain += GetPartialGainFrac(first_shape_, second_shape_, fraction_, idx)
                * Sinc((partial_idx + 1) - (idx + 1) * sync_ratio_);
        }
        partials.gains[partial_idx] = gain;
    }
}

void Sync::OnUpdateTick(const SynthParam& params, int skip) {
    auto [a,b,c] = param::FloatChoiceParam<param::Sync_WaveShape, param::Sync_WaveShape::WaveShape>::GetInterpIndex(
        params.timber.arg0
    );
    first_shape_ = a;
    second_shape_ = b;
    fraction_ = c;
    
    auto semitone = param::FloatParam<param::Sync_Sync>::GetNumber(params.timber.arg1);
    sync_ratio_ = std::exp2(semitone / 12.0f);
}

void Sync::OnNoteOn(int note) {
}

void Sync::OnNoteOff() {
}
}