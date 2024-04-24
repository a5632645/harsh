#include "effect_layout.h"

#include "param/param.h"
#include "param/effect_param.h"

namespace mana {
constexpr std::array kOctaveTitles{
    param::Octaver_Amount::kName,
    param::Octaver_Width::kName,
    param::Octaver_Decay::kName
};
constexpr std::array kOctaveTextGetters{
    param::FloatParam<param::Octaver_Amount>::GetText,
    param::FloatParam<param::Octaver_Width>::GetText,
    param::FloatParam<param::Octaver_Decay>::GetText
};

constexpr std::array kReverbTitles{
    param::Reverb_Amount::kName,
    param::Reverb_Decay::kName,
    param::Reverb_Attack::kName,
    param::Reverb_Damp::kName
};
constexpr std::array kReverbTextGetters{
    param::FloatParam<param::Reverb_Amount>::GetText,
    param::FloatParam<param::Reverb_Decay>::GetText,
    param::FloatParam<param::Reverb_Attack>::GetText,
    param::FloatParam<param::Reverb_Damp>::GetText
};
}

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
    OnEffectTypeChanged(synth_param_.timber.timber_type);
}

void EffectLayout::Paint() {
    synth_param_.effects[effect_idx].is_enable = is_enable_.Show();
    for (auto& knob : arg_knobs_) {
        knob.display();
    }
    effect_type_.show();
}

void EffectLayout::SetBounds(int x, int y, int w, int h) {
    is_enable_.SetBounds(rgc::Bounds(x, y, 12, 12));
    effect_type_.SetBounds(rgc::Bounds(x, y + 12, w, 16));
    auto first_y = y + 12 + 16;
    arg_knobs_[0].set_bound(x, first_y, 50, 70);
    arg_knobs_[1].set_bound(x + 50, first_y, 50, 70);
    arg_knobs_[2].set_bound(x + 100, first_y, 50, 70);
    auto second_y = first_y + 70;
    arg_knobs_[3].set_bound(x, second_y, 50, 70);
    arg_knobs_[4].set_bound(x + 50, second_y, 50, 70);
    arg_knobs_[5].set_bound(x + 100, second_y, 50, 70);
}

void EffectLayout::OnEffectTypeChanged(int c) {
    synth_param_.effects[effect_idx].effect_type = c;

    auto type = param::IntChoiceParam<param::EffectType, param::EffectType::EffectTypeEnum>::GetEnum(c);
    using enum param::EffectType::EffectTypeEnum;
    switch (type) {
    case kOctaver:
        SetKnobInfos(kOctaveTitles, kOctaveTextGetters);
        break;
    case kReverb:
        SetKnobInfos(kReverbTitles, kReverbTextGetters);
        break;
    case kChorus:
        SetKnobInfos<4>({
            param::Chorus_Amount::kName,
            param::Chorus_Depth::kName,
            param::Chorus_Offset::kName,
            param::Chorus_Speed::kName }
            , {
            param::FloatParam<param::Chorus_Amount>::GetText,
            param::FloatParam<param::Chorus_Depth>::GetText,
            param::FloatParam<param::Chorus_Offset>::GetText,
            param::FloatParam<param::Chorus_Speed>::GetText
            });
        break;
    default:
        break;
    }
}
}