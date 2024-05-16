#include "osc_layout.h"

#include "param/timber_param.h"
#include "param/param.h"
#include "gui_param_pack.h"
#include "engine/synth.h"

namespace mana {
OscLayout::OscLayout(Synth& synth, int idx)
    : timber_type_(synth.GetParamBank().GetParamPtr<IntParameter>(std::format("timber.osc{}.type", idx)))
    , idx_(idx) {
    timber_type_.SetChoices(param::TimberType::kNames);
    timber_type_.on_choice_changed = [this](int c) {OnTimberTypeChanged(c); };

    for (int i = 0; auto & k : arg_knobs_) {
        k.set_parameter(synth.GetParamBank().GetParamPtr(std::format("timber.osc{}.arg{}", idx, i++)));
    }
    OnTimberTypeChanged(0);
}

void OscLayout::Paint() {
    for (auto& knob : arg_knobs_) {
        knob.display();
    }
    timber_type_.Paint();
}

void OscLayout::SetBounds(int x, int y, int w, int h) {
    auto x_f = static_cast<float>(x);
    auto y_f = static_cast<float>(y);
    auto w_f = static_cast<float>(w);

    timber_type_.SetBounds(rgc::Bounds(x_f, y_f, w_f, 12));
    arg_knobs_[0].set_bound(x, y + 12, 50, 50);
    arg_knobs_[1].set_bound(x + 50, y + 12, 50, 50);
    arg_knobs_[2].set_bound(x + 100, y + 12, 50, 50);
    arg_knobs_[3].set_bound(x + 150, y + 12, 50, 50);
}

void OscLayout::OnTimberTypeChanged(int c) {
    auto type = param::TimberType::GetEnum(c);
    using enum mana::param::TimberType::ParamEnum;
    switch (type) {
    case kDualSaw:
        SetGuiKnobs(arg_knobs_,
                    param::DualSaw_Ratio{},
                    param::DualSaw_BeatingRate{},
                    param::DualSaw_SawSquare{},
                    param::DualSaw_SecondAmp{});
        break;
    case kSync:
        SetGuiKnobs(arg_knobs_,
                    param::Sync_Sync{},
                    param::Sync_WaveShape{});
        break;
    case kNoise:
        SetGuiKnobs(arg_knobs_,
                    param::Noise_Color{},
                    param::Noise_Dynamic{},
                    param::Noise_Seed{});
        break;
    case kPwm:
        SetGuiKnobs(arg_knobs_,
                    param::Pwm_Tilt{},
                    param::Pwm_Width{});
        break;
    default:
        SetGuiKnobs(arg_knobs_);
        assert(false);
        break;
    }
}
}