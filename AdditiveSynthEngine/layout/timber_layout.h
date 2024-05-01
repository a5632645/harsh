#pragma once

#include "engine/synth.h"
#include "ui/WrapDropBox.h"
#include "ui/Knob.h"

namespace mana {
class TimberLayout {
public:
    TimberLayout(Synth& synth);

    void Paint();
    void SetBounds(int x, int y, int w, int h);
private:
    void OnTimberTypeChanged(int c);

    template<size_t N>
    void SetKnobInfos(std::array<std::string_view, N> titles, std::array<std::string(*)(float), N> callers) {
        for (int i = 0; auto title : titles) {
            arg_knobs_[i++].set_title(title);
        }
        for (int i = 0; auto setter : callers) {
            arg_knobs_[i++].value_to_text_function = setter;
        }
        for (size_t i = 0; i < N; ++i) {
            arg_knobs_[i].SetEnable(true);
        }
        for (size_t i = N; i < arg_knobs_.size(); ++i) {
            arg_knobs_[i].SetEnable(false);
        }
    }

    SynthParam& synth_param_;
    WrapDropBox timber_type_;
    std::array<Knob, 4> arg_knobs_;
};
}
