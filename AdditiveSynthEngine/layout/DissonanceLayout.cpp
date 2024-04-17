#include "DissonanceLayout.h"

#include "param/param.h"
#include "param/dissonance_param.h"

namespace mana {
DissonanceLayout::DissonanceLayout(Synth& synth)
    : synth_param_(synth.GetSynthParam()) {
    type_.SetChoices(param::DissonanceType::kNames);
    type_.on_choice_changed = [this](int c) {OnDissonanceTypeChanged(c); };
    OnDissonanceTypeChanged(synth_param_.dissonance.dissonance_type);

    arg0_.set_parameter(synth.GetParamBank().GetParamPtr("dissonance.arg0"));
    arg1_.set_parameter(synth.GetParamBank().GetParamPtr("dissonance.arg1"));

    is_enable_.SetText("dissonance");
    is_enable_.SetChecked(synth_param_.dissonance.is_enable);
}

void DissonanceLayout::Paint() {
    arg0_.display();
    arg1_.display();
    synth_param_.dissonance.is_enable = is_enable_.Show();
    type_.show();
}

void DissonanceLayout::SetBounds(int x, int y, int w, int h) {
    is_enable_.SetBounds(rgc::Bounds(x, y, 12, 12));
    type_.SetBounds(rgc::Bounds(x, y + 12, w, 12));
    arg0_.set_bound(x, y + 24, 50, 70);
    arg1_.set_bound(x + 50, y + 24, 50, 70);
}

void DissonanceLayout::OnDissonanceTypeChanged(int c) {
    synth_param_.dissonance.dissonance_type = c;

    auto type = param::IntChoiceParam<param::DissonanceType, param::DissonanceType::DissonaceTypeEnum>::GetEnum(c);

    using enum param::DissonanceType::DissonaceTypeEnum;
    switch (type) {
    case kString:
        arg0_.set_title(param::StringDissStretch::kName);
        arg0_.value_to_text_function = param::FloatParam<param::StringDissStretch>::GetText;
        break;
    case kHarmonicStretch:
        arg0_.set_title(param::HarmonicStrech::kName);
        arg0_.value_to_text_function = param::FloatParam<param::HarmonicStrech>::GetText;
        break;
    case kSemitoneSpace:
        arg0_.set_title(param::SemitoneSpace::kName);
        arg0_.value_to_text_function = param::FloatParam<param::SemitoneSpace>::GetText;
        break;
    default:
        break;
    }
}
}