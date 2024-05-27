#include "wrap_slider.h"

#include "data/juce_param.h"
#include "layout/modulators/modulator_button.h"
#include "engine/synth.h"

namespace mana {
struct WrapSlider::ParamRefStore {
    virtual ~ParamRefStore() = default;

    virtual bool IsModulatable() = 0;
};

struct FloatParamRefStore : public WrapSlider::ParamRefStore {
    FloatParameter& param_;
    FloatParamRefStore(FloatParameter& param) : param_(param) {}

    bool IsModulatable() override { return param_.GetModulationType() == ModulationType::kPoly; }
};

struct IntParamRefStore : public WrapSlider::ParamRefStore {
    IntParameter& param_;
    IntParamRefStore(IntParameter& param) : param_(param) {}

    bool IsModulatable() override { return false; }
};
}

namespace mana {
WrapSlider::WrapSlider(FloatParameter* p)
    : juce::Slider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox)
    , parameter_(p->As<JuceFloatParam>().GetRef()) {
    attachment_ = std::make_unique<juce::SliderParameterAttachment>(p->As<JuceFloatParam>().GetRef(), *this);
    ref_store_ = std::make_unique<FloatParamRefStore>(*p);
    setPopupDisplayEnabled(true, true, nullptr);
}

WrapSlider::WrapSlider(IntParameter * p)
    : juce::Slider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox)
    , parameter_(p->As<JuceIntParam>().GetRef()) {
    attachment_ = std::make_unique<juce::SliderParameterAttachment>(p->As<JuceIntParam>().GetRef(), *this);
    ref_store_ = std::make_unique<IntParamRefStore>(*p);
    setPopupDisplayEnabled(true, true, nullptr);
}

WrapSlider::~WrapSlider() {
    attachment_ = nullptr;
    ref_store_ = nullptr;
}

void WrapSlider::paint(juce::Graphics& g) {
    if (shound_highlight_) {
        g.fillAll(juce::Colours::aqua);
    }
    juce::Slider::paint(g);
}

void WrapSlider::BeginHighlightModulator(std::string_view id) {
    if (ref_store_->IsModulatable()) {
        shound_highlight_ = true;
        repaint();
    }
}

void WrapSlider::StopHighliteModulator() {
    shound_highlight_ = false;
    repaint();
}

bool WrapSlider::isInterestedInDragSource(const SourceDetails& dragSourceDetails) {
    return ref_store_->IsModulatable();
}

void WrapSlider::itemDropped(const SourceDetails& dragSourceDetails) {
    auto modulator_id = dragSourceDetails.description.toString().toStdString();
    auto source_button_flag_component_ptr = dragSourceDetails.sourceComponent;
    if (source_button_flag_component_ptr.wasObjectDeleted()) {
        return;
    }

    // get the component that has the synth ref
    auto source_button_flag_component = source_button_flag_component_ptr.get();
    auto* ref_component = static_cast<ModulatorButton*>(source_button_flag_component->getParentComponent());
    auto& synth = ref_component->synth_;
    auto param_id = parameter_.paramID.toStdString();
    synth.CreateModulation(modulator_id, param_id);
}
}