#include "timber_layout.h"

#include <unordered_map>
#include "param/timber.h"
#include "param/param.h"

namespace mana {
TimberLayout::TimberLayout(Synth& synth)
    : synth_param_(synth.GetSynthParam()) {
    timber_type_.SetChoices(param::TimberType::kNames);
    timber_type_.on_choice_changed = [this](int c) {OnTimberTypeChanged(c); };

    arg0_.set_parameter(synth.GetParamBank().GetParamPtr("timber.arg0"));
    arg1_.set_parameter(synth.GetParamBank().GetParamPtr("timber.arg1"));
    arg2_.set_parameter(synth.GetParamBank().GetParamPtr("timber.arg2"));
    arg3_.set_parameter(synth.GetParamBank().GetParamPtr("timber.arg3"));

    // init
    OnTimberTypeChanged(synth_param_.timber.timber_type);
}

void TimberLayout::Paint() {
    arg0_.display();
    arg1_.display();
    arg2_.display();
    arg3_.display();
    timber_type_.show();
}

void TimberLayout::SetBounds(int x, int y, int w, int h) {
    timber_type_.SetBounds(rgc::Bounds(x, y, w, 12));
    arg0_.set_bound(x, y + 12, 50, 70);
    arg1_.set_bound(x + 50, y + 12, 50, 70);
    arg2_.set_bound(x + 100, y + 12, 50, 70);
    arg3_.set_bound(x + 150, y + 12, 50, 70);
}

void TimberLayout::OnTimberTypeChanged(int c) {
    synth_param_.timber.timber_type = c;

    auto type = param::IntChoiceParam<param::TimberType, param::TimberType::TimberEnum>::GetEnum(c);
    switch (type) {
    case mana::param::TimberType::TimberEnum::kDualSaw:
        arg0_.set_title(param::DualSaw_Ratio::kName);
        arg1_.set_title(param::DualSaw_BeatingRate::kName);
        arg2_.set_title(param::DualSaw_SawSquare::kName);

        arg0_.value_to_text_function = param::FloatParam<param::DualSaw_Ratio>::GetText;
        arg1_.value_to_text_function = param::FloatParam<param::DualSaw_BeatingRate>::GetText;
        arg2_.value_to_text_function = param::FloatParam<param::DualSaw_SawSquare>::GetText;
        break;
    case mana::param::TimberType::TimberEnum::kSync:
        arg0_.set_title(param::Sync_WaveShape::kName);
        arg1_.set_title(param::Sync_WaveShape::kName);

        arg0_.value_to_text_function = param::FloatChoiceParam<param::Sync_WaveShape, param::Sync_WaveShape::WaveShape>::GetText;
        arg1_.value_to_text_function = param::FloatParam<param::Sync_Sync>::GetText;
        break;
    }
}
}