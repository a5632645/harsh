#include "filter_layout.h"

#include <algorithm>
#include "param/filter_param.h"
#include "layout/gui_param_pack.h"
#include "engine/synth.h"

namespace mana {
mana::FilterLayout::FilterLayout(Synth & synth)
    : filter_type_(synth.GetParamBank().GetParamPtr<IntChoiceParameter>("filter.type")) {
    filter_type_.on_choice_changed = [this](int c) {OnFilterTypeChanged(c); };

    for (int i = 0; auto & knob : arg_filter_knobs_) {
        knob.set_parameter(synth.GetParamBank().GetParamPtr(std::format("filter.arg{}", i++)));
    }

    OnFilterTypeChanged(0);
    OnResonanceTypeChanged(0);
}

void FilterLayout::Paint() {
    std::ranges::for_each(arg_filter_knobs_, &Knob::display);
    filter_type_.Paint();

    using enum param::Filter_Type::ParamEnum;
    auto type = param::Filter_Type::GetEnum(filter_type_.get_item_selected());
    switch (type) {
    case kLowpass:
    case kHighpass:
    case kBandpass:
    case kBandstop:
    {
        auto reso_type = param::Filter_ResonanceType::GetChoiceIndex(arg_filter_knobs_[param::Filter_ResonanceType::kArgIdx].get_value());
        if (reso_type != last_resonance_type_) {
            last_resonance_type_ = reso_type;
            OnResonanceTypeChanged(reso_type);
        }
    }
    break;
    default:
        break;
    }
}

void FilterLayout::SetBounds(int x, int y, int w, int h) {
    auto x_f = static_cast<float>(x);
    auto y_f = static_cast<float>(y);
    auto w_f = static_cast<float>(w);

    filter_type_.SetBounds(rgc::Bounds(x_f, y_f, w_f, 16));
    arg_filter_knobs_[0].set_bound(x, y + 16, 50, 50);
    arg_filter_knobs_[1].set_bound(x + 50, y + 16, 50, 50);
    arg_filter_knobs_[2].set_bound(x + 100, y + 16, 50, 50);
    arg_filter_knobs_[3].set_bound(x + 150, y + 16, 50, 50);
    arg_filter_knobs_[4].set_bound(x, y + 86, 50, 50);
    arg_filter_knobs_[5].set_bound(x + 50, y + 86, 50, 50);
    arg_filter_knobs_[6].set_bound(x + 100, y + 86, 50, 50);
    arg_filter_knobs_[7].set_bound(x + 150, y + 86, 50, 50);
}

void FilterLayout::OnResonanceTypeChanged(int c) {
    using enum param::Filter_ResonanceType::ParamEnum;

    auto type = param::Filter_ResonanceType::GetEnum(c);
    switch (type) {
    case kRamp:
    case kCos:
    case kParabola:
        SetSingeKnobInfo(arg_filter_knobs_[param::Filter_Resonance::kArgIdx], param::Filter_Resonance{});
        SetSingeKnobInfo(arg_filter_knobs_[param::Filter_ResonanceWidth::kArgIdx], param::Filter_ResonanceWidth{});
        arg_filter_knobs_[param::Filter_PhaserResoCycles::kArgIdx].SetEnable(false);
        //SetGuiKnobs(arg_reso_knobs_,
        //            param::Filter_Resonance{},
        //            param::Filter_ResonanceWidth{});
        break;
    case kPhaser:
        SetSingeKnobInfo(arg_filter_knobs_[param::Filter_Resonance::kArgIdx], param::Filter_Resonance{});
        SetSingeKnobInfo(arg_filter_knobs_[param::Reso_PhaserResoWidth::kArgIdx], param::Reso_PhaserResoWidth{});
        SetSingeKnobInfo(arg_filter_knobs_[param::Filter_PhaserResoCycles::kArgIdx], param::Filter_PhaserResoCycles{});
        //SetGuiKnobs(arg_reso_knobs_,
        //            param::Filter_Resonance{},
        //            param::Reso_PhaserResoWidth{},
        //            param::Filter_PhaserResoCycles{});
        break;
    default:
        assert(false);
        break;
    }
}

void FilterLayout::OnFilterTypeChanged(int c) {
    using enum param::Filter_Type::ParamEnum;

    auto type = param::Filter_Type::GetEnum(c);
    switch (type) {
    case kLowpass:
    case kHighpass:
        SetGuiKnobs(arg_filter_knobs_,
                    param::Filter_Cutoff{},
                    param::Filter_Slope{},
                    param::Filter_Knee{},
                    param::Filter_ResonanceType{});
        OnResonanceTypeChanged(last_resonance_type_);
        break;
    case kBandpass:
    case kBandstop:
        SetGuiKnobs(arg_filter_knobs_,
                    param::Filter_Cutoff{},
                    param::Filter_Slope{},
                    param::Filter_BandWidth{},
                    param::Filter_Knee{},
                    param::Filter_ResonanceType{});
        OnResonanceTypeChanged(last_resonance_type_);
        break;
    case kCombFilter:
        SetGuiKnobs(arg_filter_knobs_,
                    param::Filter_Cutoff{},
                    param::Filter_CombPhase{},
                    param::Filter_CombDepth{},
                    param::Filter_CombPhaser{},
                    param::Filter_CombShape{},
                    param::Filter_KeyTracking{});
        break;
    case kPhaser:
        SetGuiKnobs(arg_filter_knobs_,
                    param::Filter_Cutoff{},
                    param::Filter_PhaserShape{},
                    param::Filter_PhaserPhase{},
                    param::Filter_PhaserNotches{},
                    param::Filter_PhaserDepth{},
                    param::Filter_PhaserWidth{});
        break;
    case kFormant:
        SetGuiKnobs(arg_filter_knobs_,
                    param::VowelFilter_Formant{},
                    param::VowelFilter_Resonance{},
                    param::VowelFilter_Select{},
                    param::VowelFilter_Singer{},
                    param::VowelFilter_Slope{});
        break;
    }
}
}