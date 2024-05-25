#pragma once

#include "engine/partials.h"
#include "engine/forward_decalre.h"
#include "engine/poly_param.h"

namespace mana {
class FormantFilter {
public:
    FormantFilter(int idx) : idx_(idx) {}

    void Init(float sample_rate, float update_rate);
    void PrepareParams(OscillorParams& params);
    void Process(Partials& partials, std::vector<float>& out);
    void OnUpdateTick();
    void OnNoteOff() {}
    void OnNoteOn(int note) {}

    struct ConvertVowlInfo {
        std::array<float, 5> cut_pitch;
        std::array<float, 5> amps;
        std::array<float, 5> p_begin;
        std::array<float, 5> p_end;
        std::array<float, 5> magic_coef;
    };
private:

    const int idx_;
    float inv_sample_rate_{};
    ConvertVowlInfo vowel_info_{};

    PolyModuFloatParameter* arg_formant_shift_{};
    PolyModuFloatParameter* arg_singer_{};
    PolyModuFloatParameter* arg_slope_{};
    PolyModuFloatParameter* arg_resonance_{};
    PolyModuFloatParameter* arg_select_{};
};
}