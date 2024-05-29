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
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.drawRect(getLocalBounds());
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