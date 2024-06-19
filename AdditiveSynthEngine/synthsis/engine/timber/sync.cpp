#include "sync.h"

#include <numbers>
#include <complex>
#include "param/param.h"
#include "param/param_helper.h"
#include "param/timber_param.h"
#include "engine/VolumeTable.hpp"

#include "table/sync_table.h"

namespace mana {
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
    std::ranges::copy(table | std::views::take(kNumPartials), frame.gains.begin());
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