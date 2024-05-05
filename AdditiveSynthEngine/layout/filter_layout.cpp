#include "filter_layout.h"

#include <algorithm>
#include "param/filter_param.h"
#include "layout/gui_param_pack.h"

namespace mana {
mana::FilterLayout::FilterLayout(Synth & synth)
    : synth_param_(synth.GetSynthParam()) {
    filter_type_.SetChoices(param::Filter_Type::kNames);
    filter_type_.on_choice_changed = [this](int c) {OnFilterTypeChanged(c); };

    for (int i = 0; auto & knob : arg_knobs_) {
        knob.set_parameter(synth.GetParamBank().GetParamPtr(std::format("filter.arg{}", i++)));
    }

    // init
    OnFilterTypeChanged(synth_param_.filter.filter_type);
}

void FilterLayout::Paint() {
    std::ranges::for_each(arg_knobs_, &Knob::display);
    filter_type_.show();
}

void FilterLayout::SetBounds(int x, int y, int w, int h) {
    auto x_f = static_cast<float>(x);
    auto y_f = static_cast<float>(y);
    auto w_f = static_cast<float>(w);

    filter_type_.SetBounds(rgc::Bounds(x_f, y_f, w_f, 16));
    arg_knobs_[0].set_bound(x, y + 16, 50, 70);
    arg_knobs_[1].set_bound(x + 50, y + 16, 50, 70);
    arg_knobs_[2].set_bound(x + 100, y + 16, 50, 70);
    arg_knobs_[3].set_bound(x, y + 86, 50, 70);
    arg_knobs_[4].set_bound(x + 50, y + 86, 50, 70);
    arg_knobs_[5].set_bound(x + 100, y + 86, 50, 70);
}

void FilterLayout::OnFilterTypeChanged(int c) {
    synth_param_.filter.filter_type = c;

    auto type = param::Filter_Type::GetEnum(c);
    switch (type) {
    case mana::param::Filter_Type::ParamEnum::kLowpass:
        SetGuiKnobs(arg_knobs_,
                    param::Filter_Cutoff{},
                    param::Filter_Resonance{},
                    param::Filter_Slope{});
        break;
    case mana::param::Filter_Type::ParamEnum::kCombFilter:
        SetGuiKnobs(arg_knobs_,
                    param::Filter_Cutoff{},
                    param::Filter_CombPhase{},
                    param::Filter_CombDepth{},
                    param::Filter_CombPhaser{},
                    param::Filter_CombShape{},
                    param::Filter_KeyTracking{});
        break;
    case mana::param::Filter_Type::ParamEnum::kPhaser:
        SetGuiKnobs(arg_knobs_,
                    param::Filter_Cutoff{},
                    param::Filter_PhaserShape{},
                    param::Filter_PhaserPhase{},
                    param::Filter_PhaserNotches{},
                    param::Filter_PhaserDepth{},
                    param::Filter_PhaserWidth{});
        break;
    }
}
}