#include "DissonanceLayout.h"

#include <cassert>
#include "param/param.h"
#include "param/dissonance_param.h"
#include "layout/gui_param_pack.h"
#include "engine/synth.h"

namespace mana {
DissonanceLayout::DissonanceLayout(Synth& synth)
    : is_enable_(synth.GetParamBank().GetParamPtr<BoolParameter>("dissonance.enable"))
    , type_(synth.GetParamBank().GetParamPtr<IntChoiceParameter>("dissonance.type")) {
    type_.SetChoices(param::DissonanceType::kNames);
    type_.on_choice_changed = [this](int c) {OnDissonanceTypeChanged(c); };

    arg_knobs_[0].set_parameter(synth.GetParamBank().GetParamPtr("dissonance.arg0"));
    arg_knobs_[1].set_parameter(synth.GetParamBank().GetParamPtr("dissonance.arg1"));

    is_enable_.SetText("dissonance");
    OnDissonanceTypeChanged(0);
}

void DissonanceLayout::Paint() {
    is_enable_.Paint();
    if (!is_enable_.IsChecked()) {
        return;
    }

    for (auto& k : arg_knobs_) {
        k.display();
    }
    type_.Paint();
}

void DissonanceLayout::SetBounds(int x, int y, int w, int h) {
    auto x_f = static_cast<float>(x);
    auto y_f = static_cast<float>(y);
    auto w_f = static_cast<float>(w);

    is_enable_.SetBounds(rgc::Bounds(x_f, y_f, 12.0f, 12.0f));
    type_.SetBounds(rgc::Bounds(x_f, y_f + 12.0f, w_f, 12.0f));
    arg_knobs_[0].set_bound(x, y + 24, 50, 50);
    arg_knobs_[1].set_bound(x + 50, y + 24, 50, 50);
}

void DissonanceLayout::OnDissonanceTypeChanged(int c) {
    auto type = param::DissonanceType::GetEnum(c);

    using enum param::DissonanceType::ParamEnum;
    switch (type) {
    case kString:
        SetGuiKnobs(arg_knobs_,
                    param::StringDissStretch{},
                    param::StringMultiRatio{});
        break;
    case kHarmonicStretch:
        SetGuiKnobs(arg_knobs_,
                    param::HarmonicStrech{});
        break;
    case kSemitoneSpace:
        SetGuiKnobs(arg_knobs_,
                    param::SemitoneSpace{});
        break;
    case kStaticError:
        SetGuiKnobs(arg_knobs_,
                    param::ErrorRamp{},
                    param::ErrorRange{});
        break;
    case kFakeUnison:
    case kFakeUnison2:
        SetGuiKnobs(arg_knobs_,
                    param::FakeUnisonRatio0{},
                    param::FakeUnisonRatio1{});
        break;
    case kDispersion:
        SetGuiKnobs(arg_knobs_,
                    param::Dispersion_Amount{},
                    param::Dispersion_Warp{});
        break;
    case kPitchQuantize:
        SetGuiKnobs(arg_knobs_,
                    param::PitchQuantize_Amount{});
        break;
    default:
        assert(false && "unkown type");
        break;
    }
}
}