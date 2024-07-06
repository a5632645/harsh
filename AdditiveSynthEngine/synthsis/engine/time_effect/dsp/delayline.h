#pragma once

#include <vector>

namespace mana::dsp {
template<std::floating_point T>
class DelayLine {
public:
    void SetMaxSize(int size) {
        max_size_ = size;
        buffer_.resize(size + 3);
    }

    T Process(T in, T delay_in_sample) {
        auto read_pos = write_pos_ - delay_in_sample;
        if (read_pos < 0.0f) {
            read_pos += max_size_;
        }
        auto curr = static_cast<int>(read_pos);
        auto next = curr + 1;
        auto frac = read_pos - curr;
        buffer_[write_pos_] = in;
        buffer_[max_size_] = buffer_.front();
        write_pos_ = (write_pos_ + 1) % max_size_;
        auto out = buffer_[curr] + frac * (buffer_[next] - buffer_[curr]);
        return out;
    }
private:
    std::vector<T> buffer_;
    int max_size_{};
    int write_pos_{};
};
}