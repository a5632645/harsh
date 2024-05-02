#include "resynthsis_layout.h"

#include <format>
#include "param/resynthsis_param.h"
#include "layout/gui_param_pack.h"

namespace mana {
ResynthsisLayout::ResynthsisLayout(Synth& synth)
    : synth_(synth) {
    for (int i = 0; auto & k : arg_knobs_) {
        k.set_parameter(synth.GetParamBank().GetParamPtr(std::format("resynthsis.arg{}", i++)));
    }

    SetGuiKnobs(arg_knobs_,
                param::Resynthsis_FormantMix{},
                param::Resynthsis_FormantShift{},
                param::Resynthsis_FrameOffset{},
                param::Resynthsis_FrameSpeed{},
                param::Resynthsis_FreqScale{},
                param::Resynthsis_GainMix{});

    is_enable_.SetText("resynthsis");
    is_enable_.SetChecked(synth.GetSynthParam().resynthsis.is_enable);
}

void ResynthsisLayout::Paint() {
    synth_.GetSynthParam().resynthsis.is_enable = is_enable_.Show();
    std::ranges::for_each(arg_knobs_, &Knob::display);
}

void ResynthsisLayout::SetBounds(int x, int y, int w, int h) {
    auto x_f = static_cast<float>(x);
    auto y_f = static_cast<float>(y);
    auto w_f = static_cast<float>(w);
    is_enable_.SetBounds(rgc::Bounds(x_f, y_f, 12.0f, 12.0f));

    for (int i = 0; auto & k : arg_knobs_) {
        k.set_bound(x + 50 * i, y + 12, 50, 70);
        ++i;
    }
}
}