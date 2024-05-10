#include "timber_layout.h"

#include <unordered_map>
#include "param/timber_param.h"
#include "param/param.h"
#include "gui_param_pack.h"

namespace mana {
TimberLayout::TimberLayout(Synth& synth)
    : osc1_layout_(synth, 0)
    , osc2_layout_(synth, 1) {
    osc2_beating_.set_parameter(synth.GetParamBank().GetParamPtr("timber.osc2_beating"));
    osc2_shift_.set_parameter(synth.GetParamBank().GetParamPtr("timber.osc2_shift"));
    osc1_gain_.set_parameter(synth.GetParamBank().GetParamPtr("timber.osc1_gain"));
    osc2_gain_.set_parameter(synth.GetParamBank().GetParamPtr("timber.osc2_gain"));

    SetSingeKnobInfo(osc2_beating_, param::Timber_Osc2Beating{});
    SetSingeKnobInfo(osc2_shift_, param::Timber_Osc2Shift{});
    SetSingeKnobInfo(osc1_gain_, param::Timber_OscGain{});
    SetSingeKnobInfo(osc2_gain_, param::Timber_OscGain{});
    osc1_gain_.set_title("osc1_gain");
    osc2_gain_.set_title("osc2_gain");
}

void TimberLayout::Paint() {
    osc2_beating_.display();
    osc2_shift_.display();
    osc1_gain_.display();
    osc2_gain_.display();
    osc1_layout_.Paint();
    osc2_layout_.Paint();
}

void TimberLayout::SetBounds(int x, int y, int w, int h) {
    auto x_f = static_cast<float>(x);
    auto y_f = static_cast<float>(y);
    auto w_f = static_cast<float>(w);

    osc2_beating_.set_bound(x + 10, y, 30, 50);
    osc2_shift_.set_bound(x + 10, y + 50, 30, 40);
    osc1_gain_.set_bound(x + 10, y + 95, 30, 30);
    osc2_gain_.set_bound(x + 10, y + 130, 30, 30);
    osc1_layout_.SetBounds(x + 50, y, 200, 70);
    osc2_layout_.SetBounds(x + 50, y + 70 + 16, 200, 70);
}
}