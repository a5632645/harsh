#include "sync.h"

#include <numbers>
#include "param/param.h"
#include "param/timber_param.h"
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
static constexpr auto kSawTable = makeHarmonicArray<kNumPartials>([](float v) {
    return  1.0f / v / (std::numbers::pi_v<float> / 2.0f);
});
static constexpr auto kSquareTable = makeHarmonicArray<kNumPartials>([](float v) {
    if ((static_cast<int>(v) & 1) == 1) {
        return  1.0f / v;
    }
    else {
        return 0.0f;
    }
});

static constexpr float GetPartialGain(param::Sync_WaveShape::ParamEnum a, int idx) {
    switch (a) {
    case mana::param::Sync_WaveShape::ParamEnum::kSine:
        return kSineTable[idx];
    case mana::param::Sync_WaveShape::ParamEnum::kTriangle:
        return kTriTable[idx];
    case mana::param::Sync_WaveShape::ParamEnum::kSaw:
        return kSawTable[idx];
    case mana::param::Sync_WaveShape::ParamEnum::kSquare:
        return kSquareTable[idx];
    default:
        return 0.0f;
    }
}

static constexpr float GetPartialGainFrac(param::Sync_WaveShape::ParamEnum a,
                                          param::Sync_WaveShape::ParamEnum b,
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

void Sync::Init(float sample_rate, float update_rate) {
}

void Sync::Process(TimberFrame& frame) {
    for (int partial_idx = 0; partial_idx < kNumPartials; ++partial_idx) {
        auto gain = 0.0f;

        for (int i = 1 - kNumPartials; i < kNumPartials; ++i) {
            auto idx = partial_idx + i;
            if (idx < 0 || idx >= kNumPartials) {
                continue;
            }

            auto w = (partial_idx + 1) - (idx + 1) * sync_ratio_;
            auto sinc_v = Sinc(w);
            gain += GetPartialGainFrac(first_shape_, second_shape_, fraction_, idx) * sinc_v;
        }
        frame.gains[partial_idx] = gain;
    }
}

void Sync::OnUpdateTick(OscParam& params) {
    auto [a, b, c] = param::Sync_WaveShape::GetInterpIndex(params.args);
    first_shape_ = a;
    second_shape_ = b;
    fraction_ = c;

    auto semitone = param::Sync_Sync::GetNumber(params.args);
    sync_ratio_ = std::exp2(semitone / 12.0f);
}

void Sync::OnNoteOn(int note) {
}

void Sync::OnNoteOff() {
}
}