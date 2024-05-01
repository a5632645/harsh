#include "timber_layout.h"

#include <unordered_map>
#include "param/timber_param.h"
#include "param/param.h"
#include "gui_param_pack.h"

namespace mana {
TimberLayout::TimberLayout(Synth& synth)
    : synth_param_(synth.GetSynthParam()) {
    timber_type_.SetChoices(param::TimberType::kNames);
    timber_type_.on_choice_changed = [this](int c) {OnTimberTypeChanged(c); };

    for (int i = 0; auto & k : arg_knobs_) {
        k.set_parameter(synth.GetParamBank().GetParamPtr(std::format("timber.arg{}", i++)));
    }

    // init
    OnTimberTypeChanged(synth_param_.timber.timber_type);
}

void TimberLayout::Paint() {
    for (auto& knob : arg_knobs_) {
        knob.display();
    }
    timber_type_.show();
}

void TimberLayout::SetBounds(int x, int y, int w, int h) {
    auto x_f = static_cast<float>(x);
    auto y_f = static_cast<float>(y);
    auto w_f = static_cast<float>(w);

    timber_type_.SetBounds(rgc::Bounds(x_f, y_f, w_f, 12));
    arg_knobs_[0].set_bound(x, y + 12, 50, 70);
    arg_knobs_[1].set_bound(x + 50, y + 12, 50, 70);
    arg_knobs_[2].set_bound(x + 100, y + 12, 50, 70);
    arg_knobs_[3].set_bound(x + 150, y + 12, 50, 70);
}

void TimberLayout::OnTimberTypeChanged(int c) {
    synth_param_.timber.timber_type = c;

    auto type = param::TimberType::GetEnum(c);
    using enum mana::param::TimberType::ParamEnum;
    switch (type) {
    case kDualSaw:
        SetGuiKnobs(arg_knobs_,
                    param::DualSaw_Ratio{},
                    param::DualSaw_BeatingRate{},
                    param::DualSaw_SawSquare{});
        break;
    case kSync:
        SetGuiKnobs(arg_knobs_,
                    param::Sync_Sync{},
                    param::Sync_WaveShape{});
        break;
    }
}
}