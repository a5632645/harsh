#include "sync.h"

#include <numbers>
#include <complex>
#include "param/param.h"
#include "param/param_helper.h"
#include "param/timber_param.h"
#include "engine/VolumeTable.hpp"

#include "table/sync_table.h"

namespace mana {
//static constexpr auto kSineTable = makeHarmonicArray<kNumPartials>([](float x) {return x == 1.0f ? 1.0f : 0.0f; });
//static constexpr auto kTriTable = makeHarmonicArray<kNumPartials>([](float x) {
//    auto xint = static_cast<int>(x);
//    if ((xint & 1) == 0) {
//        return 0.0f;
//    }
//
//    auto x2 = (xint - 1) / 2;
//    auto sign = 1.0f;
//    if ((x2 & 1) == 1) {
//        sign = -1.0f;
//    }
//    auto pi2 = std::numbers::pi_v<float> *std::numbers::pi_v<float>;
//    return 8.0f * sign / pi2 / xint / xint;
//});
//static constexpr auto kSawTable = makeHarmonicArray<kNumPartials>([](float v) {
//    return  1.0f / v / (std::numbers::pi_v<float> / 2.0f);
//});
//static constexpr auto kSquareTable = makeHarmonicArray<kNumPartials>([](float v) {
//    if ((static_cast<int>(v) & 1) == 1) {
//        return  1.0f / v;
//    }
//    else {
//        return 0.0f;
//    }
//});
//
//static constexpr float GetPartialGain(param::Sync_WaveShape::ParamEnum a, int idx) {
//    switch (a) {
//    case mana::param::Sync_WaveShape::ParamEnum::kSine:
//        return kSineTable[idx];
//    case mana::param::Sync_WaveShape::ParamEnum::kTriangle:
//        return kTriTable[idx];
//    case mana::param::Sync_WaveShape::ParamEnum::kSaw:
//        return kSawTable[idx];
//    case mana::param::Sync_WaveShape::ParamEnum::kSquare:
//        return kSquareTable[idx];
//    default:
//        return 0.0f;
//    }
//}
//
//static constexpr float GetPartialGainFrac(param::Sync_WaveShape::ParamEnum a,
//                                          param::Sync_WaveShape::ParamEnum b,
//                                          float frac,
//                                          int idx) {
//    auto a_gain = GetPartialGain(a, idx);
//    auto b_gain = GetPartialGain(b, idx);
//    return std::lerp(a_gain, b_gain, frac);
//}
//
//static constexpr float Sinc_cp(float x) {
//    x -= static_cast<int>(x);
//    if (x < 1e-5) {
//        return 1.0f;
//    }
//    return gcem::sin(std::numbers::pi_v<float> *x) / (std::numbers::pi_v<float> *x);
//}
//
//template<size_t N>
//static constexpr auto SyncConvolution_cp(std::array<float, N> arr, float sync_ratio) {
//    std::array<float, N> output{};
//    for (int partial_idx = 0; partial_idx < N; ++partial_idx) {
//        auto gain = 0.0f;
//        for (int i = 1 - kNumPartials; i < kNumPartials; ++i) {
//            auto idx = partial_idx + i;
//            if (idx < 0 || idx >= kNumPartials) {
//                continue;
//            }
//
//            if (arr[idx] <= 1e-5)
//                continue;
//
//            auto w = (partial_idx + 1) - (idx + 1) * sync_ratio;
//            auto sinc_v = 1.0f;
//            //auto sinc_v = Sinc_cp(w);
//            gain += arr[idx] * sinc_v;
//        }
//        output[partial_idx] = gain;
//    }
//    return output;
//}
//
//static constexpr auto test = SyncConvolution_cp(kSineTable, 1.0f);
//
//static float Sinc(float x) {
//    if (x == 0.0f) {
//        return 1.0f;
//    }
//    return std::sin(std::numbers::pi_v<float> *x) / (std::numbers::pi_v<float> *x);
//}

static constexpr decltype(auto) GetTable(param::Sync_WaveShape::ParamEnum a) {
    switch (a) {
    case mana::param::Sync_WaveShape::ParamEnum::kSine:
        return (kSineTable);
    case mana::param::Sync_WaveShape::ParamEnum::kTriangle:
        return (kTriTable);
    case mana::param::Sync_WaveShape::ParamEnum::kSquare:
        return (kSquareTable);
    case mana::param::Sync_WaveShape::ParamEnum::kSaw:
        return (kSawTable);
    default:
        assert(false);
        return (kSineTable);
    }
}

void Sync::Init(float sample_rate, float update_rate) {
}

void Sync::Process(TimberFrame& frame) {
    auto scale_idx = static_cast<int>(std::round(sync_ratio_ * 10.0f) - 10);
    auto clamp_idx = std::clamp(scale_idx, 0, 630);

    const auto& table = GetTable(first_shape_)[clamp_idx];
    std::ranges::copy(table, frame.gains.begin());
}

void Sync::OnUpdateTick(OscParam& params) {
    auto [a, b, c] = param::Sync_WaveShape::GetInterpIndex(params.args[param::Sync_WaveShape::kArgIdx]->Get01Value());
    first_shape_ = a;
    second_shape_ = b;
    fraction_ = c;

    auto semitone = helper::GetAlterParamValue(params.args, param::Sync_Sync{});
    sync_ratio_ = std::exp2(semitone / 12.0f);
}

void Sync::OnNoteOn(int note) {
}

void Sync::OnNoteOff() {
}
}