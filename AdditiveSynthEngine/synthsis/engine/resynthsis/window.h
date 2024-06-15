#pragma once

#include <vector>
#include <numbers>
#include <algorithm>
#include <cmath>

namespace mana {
template<typename Ftype>
class Window {
public:
    void Init(int window_length) {
        window_length_ = window_length;
        window_.resize(window_length_);
    }

    void ApplyWindow(std::vector<Ftype>& data) {
        for (int i = 0; i < window_length_; ++i) {
            data[i] *= window_[i];
        }
    }

    void FillHamming() {
        for (int i = 0; i < window_length_; ++i) {
            window_[i] = static_cast<Ftype>(0.54 - 0.46 * std::cos(std::numbers::pi_v<double> * 2 *i / (window_length_ - 1.0)));
        }
    }

    void FillBackman() {
        for (int i = 0; i < window_length_; ++i) {
            auto nor = i / (window_length_ - 1.0);
            window_[i] = static_cast<Ftype>(0.42 
                                            - 0.5 * std::cos(std::numbers::pi_v<double> *2 * nor) 
                                            + 0.08 * std::cos(std::numbers::pi_v<float> * 8 * nor));
        }
    }
private:
    int window_length_{};
    std::vector<Ftype> window_;
};

}