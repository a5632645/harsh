#include "lfo_layout.h"

#include <format>
#include "layout/gui_param_pack.h"
#include "param/lfo_param.h"
#include "engine/synth.h"

namespace mana {
LfoLayout::LfoLayout(Synth& synth, int idx) {
    const auto& bank = synth.GetParamBank();

    rate_ = std::make_unique<WrapSlider>(bank.GetParamPtr("lfo{}.rate", idx));
    start_phase_ = std::make_unique<WrapSlider>(bank.GetParamPtr(param::LFO_Phase::kIdFormater, idx));
    mode_type_ = std::make_unique<WrapDropBox>(bank.GetParamPtr<IntChoiceParameter>(param::LFO_Mode::kIdFormater, idx));
    mode_type_->addListener(this);
    time_type_ = std::make_unique<WrapDropBox>(bank.GetParamPtr<IntChoiceParameter>(param::LFO_TimeType::kIdFormater, idx));
    time_type_->addListener(this);
    wave_type_ = std::make_unique<WrapDropBox>(bank.GetParamPtr<IntChoiceParameter>(param::LFO_WaveType::kIdFormater, idx));

    addAndMakeVisible(rate_.get());
    addAndMakeVisible(start_phase_.get());
    addAndMakeVisible(mode_type_.get());
    addAndMakeVisible(time_type_.get());
    addAndMakeVisible(wave_type_.get());

    comboBoxChanged(mode_type_.get());
    comboBoxChanged(time_type_.get());
}

void LfoLayout::resized() {
    auto b = juce::Rectangle{ 0,0,100,20 };
    time_type_->setBounds(b);
    b.translate(100, 0);
    mode_type_->setBounds(b);
    b.translate(100, 0);
    wave_type_->setBounds(b);

    rate_->setBounds(time_type_->getBounds().translated(0, 20).withSize(50, 50));
    start_phase_->setBounds(mode_type_->getBounds().translated(0, 20).withSize(50, 50));
}

void LfoLayout::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) {
    if (comboBoxThatHasChanged == time_type_.get()) {
        auto t = param::LFO_TimeType::GetEnum(time_type_->getSelectedItemIndex());
        using tt = param::LFO_TimeType::ParamEnum;
        switch (t) {
        case tt::kHz:
            SetSingeKnobInfo(rate_, param::LFO_HzRate{});
            break;
        case tt::kBeats:
            SetSingeKnobInfo(rate_, param::LFO_BeatRate{});
            break;
        case tt::kSeconds:
            SetSingeKnobInfo(rate_, param::LFO_SecondRate{});
            break;
        default:
            jassertfalse;
            break;
        }
    }
    else if (comboBoxThatHasChanged == mode_type_.get()) {
        auto t = param::LFO_Mode::GetEnum(mode_type_->getSelectedItemIndex());
        using mt = param::LFO_Mode::ParamEnum;
        switch (t) {
        case mt::kFree:
            start_phase_->setVisible(false);
            break;
        case mt::kOnce:
        case mt::kReset:
            start_phase_->setVisible(true);
            break;
        default:
            jassertfalse;
            break;
        }
    }
}
}