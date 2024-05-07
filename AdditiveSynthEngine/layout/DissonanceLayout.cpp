#include "DissonanceLayout.h"

#include <cassert>
#include "param/param.h"
#include "param/dissonance_param.h"
#include "layout/gui_param_pack.h"

namespace mana {
DissonanceLayout::DissonanceLayout(Synth& synth)
    : synth_param_(synth.GetSynthParam()) {
    type_.SetChoices(param::DissonanceType::kNames);
    type_.on_choice_changed = [this](int c) {OnDissonanceTypeChanged(c); };
    OnDissonanceTypeChanged(synth_param_.dissonance.dissonance_type);

    arg_knobs_[0].set_parameter(synth.GetParamBank().GetParamPtr("dissonance.arg0"));
    arg_knobs_[1].set_parameter(synth.GetParamBank().GetParamPtr("dissonance.arg1"));

    is_enable_.SetText("dissonance");
    is_enable_.SetChecked(synth_param_.dissonance.is_enable);
}

void DissonanceLayout::Paint() {
    synth_param_.dissonance.is_enable = is_enable_.Show();
    if (!is_enable_.IsChecked()) {
        return;
    }

    for (auto& k : arg_knobs_) {
        k.display();
    }
    type_.show();
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
    synth_param_.dissonance.dissonance_type = c;

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
    default:
        assert(false && "unkown type");
        break;
    }
}
}