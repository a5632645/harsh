#pragma once

#include "engine/IProcessor.h"
#include "param/effect_param.h"
#include "param/param.h"
#include <array>
#include <complex>
#include <vector>

namespace mana {
/**
* spectral delay are not available because additive synthesizer can not play
* the whole spectrum.if use ifft that maybe possible.
 */
class HarmonicDelay : public IProcessor {
public:
    void Init(float sample_rate, float update_rate) override {
        constexpr auto max_delay_seconds = param::Delay_Time::kMax / 1000.0f;
        auto buffer_size = static_cast<int>(max_delay_seconds * update_rate);
        delay_buffer_.resize(buffer_size + 1);
        update_rate_ = update_rate;
    }

    void Process(Partials& partials) override {
        int buffer_size = static_cast<int>(delay_buffer_.size());
        int read_pos = write_pos_ - static_cast<int>(frame_offset_);
        read_pos = (read_pos + buffer_size) % buffer_size;
        auto& write_frame = delay_buffer_[write_pos_];
        const auto& read_frame = delay_buffer_.at(read_pos);
        write_pos_ = (write_pos_ + 1) % buffer_size;

        for (int i = 0; i < kNumPartials; ++i) {
            auto delay_in = partials.gains[i] * partials.phases[i] + last_write_.polar_vector[i] * feedback_;
            write_frame.polar_vector[i] = delay_in;

            auto delay_vector = read_frame.polar_vector[i];
            last_write_.polar_vector[i] = delay_vector;
            partials.gains[i] = std::abs(delay_vector);
        }
    }

    void OnUpdateTick(const SynthParam& params, int skip, int module_idx) override {
        delay_time_ = param::Delay_Time::GetNumber(params.effects[module_idx].args);
        feedback_ = param::Delay_Feedback::GetNumber(params.effects[module_idx].args);
        frame_offset_ = delay_time_ * update_rate_ / 1000.0f;
    }

    void OnNoteOn(int note) override {}

    void OnNoteOff() override {}
private:
    struct DelayFrame {
        std::array<std::complex<float>, kNumPartials> polar_vector{};
    };

    std::vector<DelayFrame> delay_buffer_;
    DelayFrame last_write_;
    float delay_time_{};
    float frame_offset_{};
    float update_rate_{};
    float feedback_{};
    int write_pos_{};
};
}