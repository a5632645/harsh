#include "timber_layout.h"

#include <unordered_map>
#include "param/timber.h"
#include "param/param.h"
#include "gui_param_pack.h"

namespace mana {
TimberLayout::TimberLayout(Synth& synth)
    : synth_param_(synth.GetSynthParam()) {
    timber_type_.SetChoices(param::TimberType::kNames);
    timber_type_.on_choice_changed = [this](int c) {OnTimberTypeChanged(c); };

    arg_knobs_[0].set_parameter(synth.GetParamBank().GetParamPtr("timber.arg0"));
    arg_knobs_[1].set_parameter(synth.GetParamBank().GetParamPtr("timber.arg1"));
    arg_knobs_[2].set_parameter(synth.GetParamBank().GetParamPtr("timber.arg2"));
    arg_knobs_[3].set_parameter(synth.GetParamBank().GetParamPtr("timber.arg3"));

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
    timber_type_.SetBounds(rgc::Bounds(x, y, w, 12));
    arg_knobs_[0].set_bound(x, y + 12, 50, 70);
    arg_knobs_[1].set_bound(x + 50, y + 12, 50, 70);
    arg_knobs_[2].set_bound(x + 100, y + 12, 50, 70);
    arg_knobs_[3].set_bound(x + 150, y + 12, 50, 70);
}

void TimberLayout::OnTimberTypeChanged(int c) {
    synth_param_.timber.timber_type = c;

    auto type = param::IntChoiceParam<param::TimberType, param::TimberType::TimberEnum>::GetEnum(c);
    using enum mana::param::TimberType::TimberEnum;
    switch (type) {
    case kDualSaw:
        SetKnobInfos<3>({
            param::DualSaw_Ratio::kName,
            param::DualSaw_BeatingRate::kName,
            param::DualSaw_SawSquare::kName }
            , {
                param::FloatParam<param::DualSaw_Ratio>::GetText,
                param::FloatParam<param::DualSaw_BeatingRate>::GetText,
                param::FloatParam<param::DualSaw_SawSquare>::GetText
            });
            break;
    case kSync:
        SetKnobInfos<2>({
            param::Sync_WaveShape::kName,
            param::Sync_Sync::kName }
            , {
                param::FloatChoiceParam<param::Sync_WaveShape, param::Sync_WaveShape::WaveShape>::GetText,
                param::FloatParam<param::Sync_Sync>::GetText
            });
            break;
    case kResynthsis:
        SetGuiKnobs(arg_knobs_,
                    param::FloatParam<param::Resynthsis_FormantMix>{},
                    param::FloatParam<param::Resynthsis_FormantShift>{},
                    param::FloatParam<param::Resynthsis_FreqScale>{},
                    param::FloatParam<param::Resynthsis_FramePos>{});
        break;
    }
}
}