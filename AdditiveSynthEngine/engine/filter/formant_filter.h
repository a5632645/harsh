#pragma once

#include "engine/partials.h"
#include "engine/forward_decalre.h"
#include "engine/poly_param.h"

namespace mana {
class FormantFilter {
public:
    void Init(float sample_rate, float update_rate);
    void PrepareParams(OscillorParams& params);
    void Process(Partials& partials);
    void OnUpdateTick();
    void OnNoteOff() {}
    void OnNoteOn(int note) {}
private:
    struct SimpleRampResonator {
        void SetParam(float slope, float cut, float bw_p, float amp_v) {
            s = slope;
            c = cut;
            bw = std::max(bw_p, 0.01f);
            a = amp_v;
        }

        float Filter(float p) {
            if (p < c) {
                auto zero_p = c - bw;
                return a + s * (p - zero_p);
            }
            else {
                auto zero_p = c + bw;
                return a - s * (p - zero_p);
            }
        }

        float s;
        float c;
        float bw;
        float a;
    };

    PolyModuFloatParameter* arg_formant_shift_{};
    PolyModuFloatParameter* arg_singer_{};
    PolyModuFloatParameter* arg_slope_{};
    PolyModuFloatParameter* arg_resonance_{};
    PolyModuFloatParameter* arg_select_{};
    std::array<SimpleRampResonator, 5> resos_{};

    float inv_sample_rate_{};
};
}