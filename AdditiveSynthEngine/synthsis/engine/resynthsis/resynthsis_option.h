#pragma once

namespace mana {
struct ResynthsisOption {
    enum class Window {
        kKaiser = 0,
        kBlackman,
        kTaylor
    } window_;

    int window_len_ = 0;
    float frequency_resolution_ = 0.0f;
    float side_lobe_level_ = 0;
    float peak_filter_level_ = 0.0f;
    float smooth_time_ = 0.0f;
    bool custom_win_len_ = false;
    bool custom_freq_res_ = false;
    bool custom_side_lobe_level_ = false;
    bool custom_peak_filter_level_ = false;
    bool custom_smooth_time_ = false;
};
}