#include "modulator_button.h"

#include "layout/main_window.h"
#include "engine/synth.h"

namespace mana {
ModulatorButton::ModulatorButton(std::string_view modulator_id, Synth& synth)
    : synth_(synth)
    , modulator_id_(modulator_id) {
    setSize(16, 16);
}

void ModulatorButton::paint(juce::Graphics& g) {
    auto radius = std::min(getWidth(), getHeight());
    auto circle_bound = juce::Rectangle{ 0,0,radius,radius }.reduced(1).withCentre(getLocalBounds().getCentre());
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(circle_bound.toFloat(), 3.0f);
    g.setColour(juce::Colours::white);
    g.drawRoundedRectangle(circle_bound.toFloat(), 3.0f, 1.0f);
}

void ModulatorButton::mouseDrag(const juce::MouseEvent& event) {
    auto* drag_container = juce::DragAndDropContainer::findParentDragContainerFor(this);
    if (drag_container == nullptr) {
        DBG("no drag container");
        return;
    }

    if (drag_container->isDragAndDropActive()) {
        return;
    }

    drag_container->startDragging(juce::String{ modulator_id_.data(), modulator_id_.length() }, this);

    // highlight
    auto main_window = findParentComponentOfClass<mana::MainWindow>();
    if (main_window != nullptr) {
        main_window->BeginHighlightModulator(modulator_id_);
    }
}

void ModulatorButton::mouseUp(const juce::MouseEvent& event) {
    auto main_window = findParentComponentOfClass<mana::MainWindow>();
    main_window->StopHighliteModulator();
}
}