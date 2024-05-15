#pragma once

#include <array>
#include <complex>
#include <vector>
#include "effect_base.h"
#include "param/effect_param.h"
#include "param/param.h"
#include "engine/modulation/curve.h"
#include "engine/synth.h"

namespace mana {
/**
* spectral delay are not available because additive synthesizer can not play
* the whole spectrum.if use ifft that maybe possible.
 */
class HarmonicDelay : public EffectBase {
public:
    void Init(float sample_rate, float update_rate) override {
        constexpr auto max_delay_seconds = param::Delay_Time::kMax / 1000.0f;
        auto buffer_size = static_cast<int>(max_delay_seconds * update_rate);
        delay_buffer_.resize(buffer_size + 1);
        update_rate_ = update_rate;
    }

    void Process(Partials& partials) override {
        //int buffer_size = static_cast<int>(delay_buffer_.size());
        //int read_pos = write_pos_ - static_cast<int>(frame_offset_);
        //read_pos = (read_pos + buffer_size) % buffer_size;
        //auto& write_frame = delay_buffer_[write_pos_];
        //const auto& read_frame = delay_buffer_.at(read_pos);

        int buffer_size = static_cast<int>(delay_buffer_.size());
        auto& write_frame = delay_buffer_[write_pos_];

        for (int i = 0; i < kNumPartials; ++i) {
            auto delay_time = delay_time_ * time_map_->data[i];
            auto frame_offset = delay_time * update_rate_ / 1000.0f;
            if (!enable_custom_time_) {
                frame_offset = frame_offset_;
            }

            int read_pos = write_pos_ - static_cast<int>(frame_offset);
            read_pos = (read_pos + buffer_size) % buffer_size;
            const auto& read_frame = delay_buffer_.at(read_pos);

            auto feedback = feedback_ * feedback_map_->data[i];
            if (!enable_custom_feedback_) {
                feedback = feedback_;
            }

            auto delay_in = partials.gains[i] * partials.phases[i] + last_write_.polar_vector[i] * feedback;
            write_frame.polar_vector[i] = delay_in;

            auto delay_vector = read_frame.polar_vector[i];
            last_write_.polar_vector[i] = delay_vector;
            partials.gains[i] = std::abs(delay_vector);
        }

        write_pos_ = (write_pos_ + 1) % buffer_size;
    }

    void OnUpdateTick(EffectParams& args, CurveManager& curves) override {
        delay_time_ = param::Delay_Time::GetNumber(args.args);
        feedback_ = param::Delay_Feedback::GetNumber(args.args);
        frame_offset_ = delay_time_ * update_rate_ / 1000.0f;
        enable_custom_time_ = param::Delay_CustomTime::GetNumber(args.args) > 0.5f;
        enable_custom_feedback_ = param::Delay_CustomFeedback::GetNumber(args.args) > 0.5f;
    }

    void PrepareParams(OscillorParams& params) override {
        time_map_ = params.GetParentSynthParams().GetCurveManager().GetCurvePtr("effect.harmonic_delay.time");
        feedback_map_ = params.GetParentSynthParams().GetCurveManager().GetCurvePtr("effect.harmonic_delay.feedback");
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
    CurveManager::Curve* time_map_{};
    CurveManager::Curve* feedback_map_{};
    bool enable_custom_time_{};
    bool enable_custom_feedback_{};
};
}