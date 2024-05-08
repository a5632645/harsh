#pragma once

#include <numbers>
#include <random>
#include <complex>
#include "engine/IProcessor.h"
#include "param/effect_param.h"
#include "param/param.h"

namespace mana {
class Scramble : public IProcessor {
public:
    void Init(float sample_rate, float update_rate) override {
        sample_rate_ = sample_rate;
    }

    void Process(Partials& partials) override {
        std::ranges::copy(partials.gains, saved_gains_.begin());

        for (int i = 0; i < kNumPartials; ++i) {
            auto target_amp = std::lerp(saved_gains_[idx_table0_[i]], saved_gains_[idx_table1_[i]], lfo_phase_);
            partials.gains[i] = target_amp;
        }
    }

    void OnUpdateTick(const SynthParam& params, int skip, int module_idx) override {
        scramble_range_ = param::Scramble_Range::GetNumber(params.effects[module_idx].args);
        lfo_rate_ = param::Scramble_Rate::GetNumber(params.effects[module_idx].args);
        UpdateLfoAndIndexTable(skip);
    }
    void OnNoteOn(int note) override {}
    void OnNoteOff() override {}
private:
    void UpdateLfoAndIndexTable(int skip) {
        float phase_inc = skip * lfo_rate_ / sample_rate_;
        lfo_phase_ += phase_inc;

        if (lfo_phase_ >= 1.0f) {
            lfo_phase_ -= 1.0f;
            ShufflerIndexTable();
        }
    }

    void ShufflerIndexTable() {
        std::ranges::copy(idx_table1_, idx_table0_.begin());
        std::ranges::copy(kInitTable, idx_table1_.begin());
        int shuffler_range = static_cast<int>(kNumPartials * scramble_range_);
        if (shuffler_range < 2) {
            return;
        }

        int idx_begin = 0;
        do {
            auto idx_end = std::min(idx_begin + shuffler_range, kNumPartials);
            std::shuffle(idx_table1_.begin() + idx_begin, idx_table1_.begin() + idx_end, rand_generator_);
            idx_begin += shuffler_range;
        } while (idx_begin < kNumPartials);
    }

    template<int... Its>
    static constexpr std::array<int, sizeof...(Its)> _MakeDefaultIndexTable(std::integer_sequence<int, Its...>) {
        return { Its... };
    }
    static constexpr auto kInitTable = _MakeDefaultIndexTable(std::make_integer_sequence<int, kNumPartials>());

    float scramble_range_{};
    float lfo_rate_{};
    float sample_rate_{};
    float lfo_phase_{ 1.0f };
    std::array<int, kNumPartials> idx_table0_{ kInitTable };
    std::array<int, kNumPartials> idx_table1_{ kInitTable };
    std::array<float, kNumPartials> saved_gains_{};
    std::random_device rand_generator_;
};
}