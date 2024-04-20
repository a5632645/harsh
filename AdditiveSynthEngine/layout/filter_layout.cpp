#include "filter_layout.h"

#include "param/filter_param.h"
#include "param/param.h"

namespace mana {
mana::FilterLayout::FilterLayout(Synth & synth)
    : synth_param_(synth.GetSynthParam()) {
    filter_type_.SetChoices(param::Filter_Type::kNames);
    filter_type_.on_choice_changed = [this](int c) {OnFilterTypeChanged(c); };

    arg0_.set_parameter(synth.GetParamBank().GetParamPtr("filter.arg0"));
    arg1_.set_parameter(synth.GetParamBank().GetParamPtr("filter.arg1"));
    arg2_.set_parameter(synth.GetParamBank().GetParamPtr("filter.arg2"));
    arg3_.set_parameter(synth.GetParamBank().GetParamPtr("filter.arg3"));
    arg4_.set_parameter(synth.GetParamBank().GetParamPtr("filter.arg4"));
    arg5_.set_parameter(synth.GetParamBank().GetParamPtr("filter.arg5"));

    // init
    OnFilterTypeChanged(synth_param_.timber.timber_type);
}

void FilterLayout::Paint() {
    arg0_.display();
    arg1_.display();
    arg2_.display();
    arg3_.display();
    arg4_.display();
    arg5_.display();
    filter_type_.show();
}

void FilterLayout::SetBounds(int x, int y, int w, int h) {
    filter_type_.SetBounds(rgc::Bounds(x, y, w, 16));
    arg0_.set_bound(x, y + 16, 50, 70);
    arg1_.set_bound(x + 50, y + 16, 50, 70);
    arg2_.set_bound(x + 100, y + 16, 50, 70);
    arg3_.set_bound(x, y + 86, 50, 70);
    arg4_.set_bound(x + 50, y + 86, 50, 70);
    arg5_.set_bound(x + 100, y + 86, 50, 70);
}

void FilterLayout::OnFilterTypeChanged(int c) {
    synth_param_.filter.filter_type = c;

    auto type = param::IntChoiceParam<param::Filter_Type, param::Filter_Type::TypeEnum>::GetEnum(c);
    switch (type) {
    case mana::param::Filter_Type::TypeEnum::kLowpass:
        arg0_.set_title(param::Filter_Cutoff::kName);
        arg1_.set_title(param::Filter_Resonance::kName);
        arg2_.set_title(param::Filter_Slope::kName);

        arg0_.value_to_text_function = param::FloatParam<param::Filter_Cutoff>::GetText;
        arg1_.value_to_text_function = param::FloatParam<param::Filter_Resonance>::GetText;
        arg2_.value_to_text_function = param::FloatParam<param::Filter_Slope>::GetText;
        break;
    case mana::param::Filter_Type::TypeEnum::kCombFilter:
        arg0_.set_title(param::Filter_Cutoff::kName);
        arg1_.set_title(param::Filter_CombShape::kName);
        arg2_.set_title(param::Filter_CombPhase::kName);
        arg3_.set_title(param::Filter_KeyTracking::kName);
        arg4_.set_title(param::Filter_CombDepth::kName);
        arg5_.set_title(param::Filter_CombPhaser::kName);

        arg0_.value_to_text_function = param::FloatParam<param::Filter_Cutoff>::GetText;
        arg1_.value_to_text_function = param::FloatParam<param::Filter_CombShape>::GetText;
        arg2_.value_to_text_function = param::FloatParam<param::Filter_CombPhase>::GetText;
        arg3_.value_to_text_function = param::FloatParam<param::Filter_KeyTracking>::GetText;
        arg4_.value_to_text_function = param::FloatParam<param::Filter_CombDepth>::GetText;
        arg5_.value_to_text_function = param::FloatParam<param::Filter_CombPhaser>::GetText;
        break;
    case mana::param::Filter_Type::TypeEnum::kPhaser:
        arg0_.set_title(param::Filter_Cutoff::kName);
        arg1_.set_title(param::Filter_PhaserShape::kName);
        arg2_.set_title(param::Filter_PhaserPhase::kName);
        arg3_.set_title(param::Filter_PhaserNotches::kName);
        arg4_.set_title(param::Filter_PhaserDepth::kName);
        arg5_.set_title(param::Filter_PhaserWidth::kName);

        arg0_.value_to_text_function = param::FloatParam<param::Filter_Cutoff>::GetText;
        arg1_.value_to_text_function = param::FloatParam<param::Filter_PhaserShape>::GetText;
        arg2_.value_to_text_function = param::FloatParam<param::Filter_PhaserPhase>::GetText;
        arg3_.value_to_text_function = param::FloatParam<param::Filter_PhaserNotches>::GetText;
        arg4_.value_to_text_function = param::FloatParam<param::Filter_PhaserDepth>::GetText;
        arg5_.value_to_text_function = param::FloatParam<param::Filter_PhaserWidth>::GetText;
        break;
    }
}
}