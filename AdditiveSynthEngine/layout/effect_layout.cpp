#include "effect_layout.h"

#include "param/param.h"
#include "param/effect_param.h"
#include "layout/gui_param_pack.h"

namespace mana {
EffectLayout::EffectLayout(Synth& synth, int effect_idx)
    : synth_param_(synth.GetSynthParam())
    , effect_idx(effect_idx) {
    effect_type_.SetChoices(param::EffectType::kNames);
    effect_type_.on_choice_changed = [this](int c) {OnEffectTypeChanged(c); };

    for (int i = 0; auto & arg_knob : arg_knobs_) {
        arg_knob.set_parameter(synth.GetParamBank().GetParamPtr(std::format("effect{}.arg{}", effect_idx, i++)));
    }

    is_enable_.SetText("effect");
    is_enable_.SetChecked(synth_param_.effects[effect_idx].is_enable);

    // init
    OnEffectTypeChanged(synth_param_.effects[effect_idx].effect_type);
}

void EffectLayout::Paint() {
    synth_param_.effects[effect_idx].is_enable = is_enable_.Show();
    if (!is_enable_.IsChecked()) {
        return;
    }

    for (auto& knob : arg_knobs_) {
        knob.display();
    }
    effect_type_.show();
}

void EffectLayout::SetBounds(int x, int y, int w, int h) {
    auto x_f = static_cast<float>(x);
    auto y_f = static_cast<float>(y);
    auto w_f = static_cast<float>(w);

    is_enable_.SetBounds(rgc::Bounds(x_f, y_f, 12, 12));
    effect_type_.SetBounds(rgc::Bounds(x_f, y_f + 12, w_f, 16));
    auto first_y = y + 12 + 16;
    arg_knobs_[0].set_bound(x, first_y, 50, 50);
    arg_knobs_[1].set_bound(x + 50, first_y, 50, 50);
    arg_knobs_[2].set_bound(x + 100, first_y, 50, 50);
    auto second_y = first_y + 70;
    arg_knobs_[3].set_bound(x, second_y, 50, 50);
    arg_knobs_[4].set_bound(x + 50, second_y, 50, 50);
    arg_knobs_[5].set_bound(x + 100, second_y, 50, 50);
}

void EffectLayout::OnEffectTypeChanged(int c) {
    synth_param_.effects[effect_idx].effect_type = c;

    auto type = param::EffectType::GetEnum(c);
    using enum param::EffectType::ParamEnum;
    switch (type) {
    case kOctaver:
        SetGuiKnobs(arg_knobs_,
                    param::Octaver_Amount{},
                    param::Octaver_Decay{},
                    param::Octaver_Width{});
        break;
    case kReverb:
        SetGuiKnobs(arg_knobs_,
                    param::Reverb_Amount{},
                    param::Reverb_Attack{},
                    param::Reverb_Damp{},
                    param::Reverb_Decay{},
                    param::Reverb_Speed{});
        break;
    case kChorus:
        SetGuiKnobs(arg_knobs_,
                    param::Chorus_Amount{},
                    param::Chorus_Depth{},
                    param::Chorus_Offset{},
                    param::Chorus_Speed{});
        break;
    case kPhaser:
        SetGuiKnobs(arg_knobs_,
                    param::Phaser_BarberRate{},
                    param::Phaser_Cycles{},
                    param::Phaser_Mix{},
                    param::Phaser_Pinch{},
                    param::Phaser_Mode{},
                    param::Phaser_Shape{});
        break;
    case kScramble:
        SetGuiKnobs(arg_knobs_,
                    param::Scramble_Range{},
                    param::Scramble_Rate{});
        break;
    case kDecay:
        SetGuiKnobs(arg_knobs_,
                    param::Decay_Slope{},
                    param::Decay_Time{});
        break;
    case kHarmonicDelay:
        SetGuiKnobs(arg_knobs_,
                    param::Delay_Feedback{},
                    param::Delay_Time{});
        break;
    default:
        SetGuiKnobs(arg_knobs_);
        break;
    }
}
}