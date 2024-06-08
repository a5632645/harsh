#include "osc_layout.h"

#include "param/timber_param.h"
#include "param/param.h"
#include "layout/gui_param_pack.h"
#include "engine/synth.h"

namespace mana {
OscLayout::OscLayout(Synth& synth, int idx)
    : idx_(idx) {
    timber_type_ = std::make_unique<WrapDropBox>(synth.GetParamBank().GetParamPtr<IntChoiceParameter>(std::format("timber.osc{}.type", idx)));
    timber_type_->addListener(this);

    for (int i = 0; auto & k : arg_knobs_) {
        k = std::make_unique<WrapSlider>(synth.GetParamBank().GetParamPtr(std::format("timber.osc{}.arg{}", idx, i++)));
    }
    OnTimberTypeChanged(0);

    addAndMakeVisible(timber_type_.get());
    for (const auto& k : arg_knobs_) {
        addAndMakeVisible(*k);
    }
}

void OscLayout::resized() {
    timber_type_->setBounds(0, 0, getWidth(), 20);
    for (int i = 0; i < 4; ++i) {
        arg_knobs_[i]->setBounds(50 * i, 20, 50, 50);
    }
}

void OscLayout::OnTimberTypeChanged(int c) {
    auto type = param::TimberType::GetEnum(c);
    using enum mana::param::TimberType::ParamEnum;
    switch (type) {
    case kDualSaw:
        SetGuiKnobs(arg_knobs_,
                    param::DualSaw_Ratio{},
                    param::DualSaw_BeatingRate{},
                    param::DualSaw_SawSquare{},
                    param::DualSaw_SecondAmp{});
        break;
    case kSync:
        SetGuiKnobs(arg_knobs_,
                    param::Sync_Sync{},
                    param::Sync_WaveShape{});
        break;
    case kNoise:
        SetGuiKnobs(arg_knobs_,
                    param::Noise_Color{},
                    param::Noise_Dynamic{},
                    param::Noise_Seed{});
        break;
    case kPwm:
        SetGuiKnobs(arg_knobs_,
                    param::Pwm_Tilt{},
                    param::Pwm_Width{});
        break;
    case kImpulse:
        SetGuiKnobs(arg_knobs_,
                    param::Impulse_NumSines{});
        break;
    default:
        SetGuiKnobs(arg_knobs_);
        assert(false);
        break;
    }
}

void OscLayout::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) {
    OnTimberTypeChanged(comboBoxThatHasChanged->getSelectedItemIndex());
}
}