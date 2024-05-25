#include "oscilloscope.h"

#include <raylib.h>

namespace mana {
Oscilloscope::Oscilloscope(const Synth& synth)
    : synth_(synth) {
}

void Oscilloscope::Paint() {
    for (int i = 0; i < w_; ++i) {
        int x = x_ + i;
        int y = y_center_ + y_scale_ * data_buffer_[i];
        DrawLine(x, y, x, y_center_, RED);
    }
}

void Oscilloscope::SetBounds(int x, int y, int w, int h) {
    x_ = x;
    w_ = w;
    y_center_ = y + h / 2;
    y_scale_ = -h / 2;
    data_buffer_.resize(w_);
    data_buffer2_.resize(w_);
}

void Oscilloscope::PushBuffer(const std::vector<float>& data) {
    if (write_pos_ == 0) {
        // filter and find zero
        auto it = data.cbegin();
        auto it2 = it + 1;

        while (it2 != data.cend()) {
            auto s1 = last_sample_ + *it;
            auto s2 = *it + *it2;
            last_sample_ = *it;

            ++it;
            ++it2;

            if (std::signbit(s1) && !std::signbit(s2)) {
                break;
            }
        }

        for (; it2 != data.cend(); ++it2) {
            //data_buffer2_[write_pos_] = *it;
            if (PushData(*it2)) {
                break;
            }
        }
    }
    else {
        // directly push back
        for (const auto s : data) {
            if (PushData(s)) {
                break;
            }
        }
    }
}

void Oscilloscope::paintChannel(juce::Graphics& g, juce::Rectangle<float> bounds, const juce::Range<float>* levels, int numLevels, int nextSample) {

}

bool Oscilloscope::PushData(float s) {
    ++counter_;

    if (counter_ != nsample_per_pixel_) {
        return false;
    }

    data_buffer2_[write_pos_++] = s;
    counter_ = 0;

    if (write_pos_ == w_) {
        std::ranges::copy(data_buffer2_, data_buffer_.begin());
        write_pos_ = 0;
        return true;
    }
    else {
        return false;
    }
}
}