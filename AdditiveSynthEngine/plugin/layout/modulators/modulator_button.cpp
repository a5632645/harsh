#include "modulator_button.h"

class SimpleFlagComponent : public juce::Component {
public:
    SimpleFlagComponent(std::string_view id)
        : modulator_id_(id) {}

    void paint(juce::Graphics& g) override {
        g.fillAll(juce::Colours::black);
        g.setColour(juce::Colours::white);
        g.drawRect(getLocalBounds());
    }

    void mouseDrag(const juce::MouseEvent& event) override {
        auto* drag_container = juce::DragAndDropContainer::findParentDragContainerFor(this);
        if (drag_container == nullptr) {
            DBG("no drag container");
            return;
        }

        if (drag_container->isDragAndDropActive()) {
            return;
        }

        drag_container->startDragging(juce::String{ modulator_id_.data(), modulator_id_.length() }, this);
    }
private:
    const std::string_view modulator_id_;
};

namespace mana {
ModulatorButton::ModulatorButton(std::string_view modulator_id, Synth& synth)
    : synth_(synth) {
    num_modulation_label_ = std::make_unique<juce::Label>();
    drag_component_ = std::make_unique<SimpleFlagComponent>(modulator_id);
    // todo: let label listen to modulator's change
    addAndMakeVisible(num_modulation_label_.get());
    addAndMakeVisible(drag_component_.get());

    setSize(20, 16);
}

ModulatorButton::~ModulatorButton() {
    num_modulation_label_ = nullptr;
    drag_component_ = nullptr;
}

void ModulatorButton::resized() {
    auto b = getLocalBounds();
    drag_component_->setBounds(b.removeFromRight(5));
    num_modulation_label_->setBounds(b);
}
}