#include "delay.h"

#include "param/timefx/delay_param.h"
#include "engine/oscillor_param.h"

namespace mana {
void Delay::Init(float sample_rate, float update_rate) {
    smooth_fb_.SetSmooth(sample_rate);
    smooth_time_.SetSmooth(sample_rate, 100.0f);
    auto num_samples = std::ceil(sample_rate * (param::DelayTimeSeconds::kMax + 0.01f));
    //delay_buffer_.resize(static_cast<size_t>(num_samples));
    delay_.SetMaxSize(static_cast<int>(num_samples));
    sample_rate_ = sample_rate;
}

void Delay::PrepareParams(ModulableParams& params) {
    enable_ = params.GetParam<BoolParameter>("timefx.delay.enable");
    time_mode_ = params.GetParam<IntChoiceParameter>(param::DelayTimeMode::kId);
    time_ = params.GetModuFloatParam("timefx.delay.time");
    bpm_ = params.GetParam<FloatParameter>("bpm");
    delay_mode_ = params.GetParam<IntChoiceParameter>(param::DelayMode::kId);
    feedback_ = params.GetModuFloatParam(param::DelayFeedback::kId);
}

void Delay::OnUpdateTick() {
    smooth_fb_.SetTarget(feedback_->GetValue());

    auto mode = param::DelayTimeMode::GetEnum(time_mode_->GetInt());
    using dtm = param::DelayTimeMode::ParamEnum;
    auto raw_time = time_->GetValue();
    auto seconds = 0.0f;
    switch (mode) {
    case dtm::kSeconds:
        seconds = param::DelayTimeSeconds::ConvertFrom01(raw_time);
        break;
    case dtm::kBeats:
    {
        auto idx = param::DelayTimeBeat::GetChoiceIndexFrom01(raw_time);
        auto beats = param::DelayTimeBeat::kNumbers[idx];
        auto secs = utli::BpmToSeconds(bpm_->GetValue(), beats);
        seconds = std::min(param::DelayTimeSeconds::kMax, secs);
        break;
    }
    default:
        assert(false);
        break;
    }
    smooth_time_.SetTarget(seconds * sample_rate_);
}

void Delay::Process(float* pbuffer, int num) {
    if (!enable_->GetBool()) return;

    //auto size = delay_buffer_.size() - 3;
    auto mode = param::DelayMode::GetEnum(delay_mode_->GetInt());
    using dm = param::DelayMode::ParamEnum;

    for (int i = 0; i < num; ++i) {
        auto sample_in = pbuffer[i];
        switch (mode) {
        case dm::kNormal:
            sample_in += last_sample_ * smooth_fb_.TickNext();
        case dm::kPingPong:
            // todo: stereo pingpong mode
            break;
        default:
            assert(false);
            break;
        }
        //delay_buffer_[write_pos_] = sample_in;

        auto delay_time = smooth_time_.TickNext();
        last_sample_ = delay_.Process(sample_in, delay_time);
        pbuffer[i] = last_sample_;
        //auto read_pos = write_pos_ - delay_time;
        //if (read_pos < 0.0f) {
            //read_pos += size;
        //}
        //auto curr = static_cast<size_t>(read_pos);
        //auto frac = read_pos - curr;
        //auto next = curr + 1;
        //auto sample_out = std::lerp(delay_buffer_[curr], delay_buffer_[next], frac);
        //last_sample_ = sample_out;
        //pbuffer[i] = sample_out;

        //++write_pos_;
        //if (write_pos_ == size) {
            //write_pos_ = 0;
        //}
    }
}
}