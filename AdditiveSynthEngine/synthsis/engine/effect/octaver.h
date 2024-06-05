#pragma once

#include <ranges>
#include <algorithm>
#include "effect_base.h"
#include "param/effect_param.h"
#include "param/param.h"
#include "param/param_helper.h"

namespace mana {
class Octaver : public EffectBase {
public:
    static constexpr auto kMaxBins = static_cast<int>(param::Octaver_Width::kMax);

    void Init(float sample_rate, float update_rate) override {}
    void Process(Partials& partials) override {
        std::ranges::transform(partials.gains, saved_gains_.begin(), [this](float v) {return v * gain_; });
        std::ranges::copy(saved_gains_, partials.gains.begin());

        auto full_num = static_cast<int>(width_);
        for (int i = 0; i < full_num; ++i) {
            ProcessBin(partials, i + 1, 1.0f);
        }

        if (full_num >= kMaxBins) {
            return;
        }

        auto last_bin = full_num + 1;
        auto last_bin_vol = width_ - static_cast<float>(full_num);
        ProcessBin(partials, last_bin, last_bin_vol);
    }

    void OnUpdateTick(EffectParams& args) override {
        amount_ = helper::GetAlterParamValue(args.args, param::Octaver_Amount{});
        width_ = helper::GetAlterParamValue(args.args, param::Octaver_Width{});
        decay_ = helper::GetAlterParamValue(args.args, param::Octaver_Decay{});
        gain_ = std::lerp(1.0f, 1.0f / (CalcAndCountTotalGain() + 1.0f), amount_);
    }
    void OnNoteOn(int note) override {}
    void OnNoteOff() override {}
private:
    float CalcAndCountTotalGain() {
        float total_gain{};
        auto full_num = static_cast<int>(width_);
        for (int i = 0; i < full_num; ++i) {
            octave_gains_[i] = Db2Gain((i + 1) * decay_);
            total_gain += octave_gains_[i];
        }

        if (full_num < kMaxBins) {
            auto last_bin = full_num + 1;
            auto last_bin_vol = width_ - static_cast<float>(full_num);
            octave_gains_[full_num] = Db2Gain(last_bin * decay_) * last_bin_vol;
            total_gain += octave_gains_[full_num];
        }

        return total_gain;
    }

    void ProcessBin(Partials& partials, int octave, float vol) {
        auto semitone = 1 << octave;
        auto org_harmonic_idx = 0;
        auto this_octave_gain = vol * amount_ * octave_gains_[octave - 1];

        for (int harmonic_idx = semitone - 1;
             harmonic_idx < kNumPartials;
             harmonic_idx += semitone) {
            auto partial_gain = saved_gains_[org_harmonic_idx] * this_octave_gain;
            partials.gains[harmonic_idx] += partial_gain;

            ++org_harmonic_idx;
        }
    }

    float Db2Gain(float db) {
        return std::exp(0.11512925464970228420089957273422f * db);
    }

    std::array<float, kNumPartials> saved_gains_{};
    std::array<float, kMaxBins> octave_gains_{};
    float amount_{};
    float width_{};
    float decay_{};
    float gain_{};
};
}