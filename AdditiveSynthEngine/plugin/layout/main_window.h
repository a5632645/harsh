#pragma once

#include "engine/forward_decalre.h"

#include "modulators/modulators_layout.h"
#include "master_layout.h"
#include "modu_listener.h"

namespace mana {
class MainWindow : public juce::Component, public juce::DragAndDropContainer {
public:
    MainWindow(Synth& synth);

    void resized() override;
    //Oscilloscope& GetWaveScope() { return master_.GetWaveScope(); }
    void BeginHighlightModulator(std::string_view id);
    void StopHighliteModulator();

    Synth& GetSynth() { return synth_; }
    void AddModulationActionListener(ModulationActionListener* listener) { listeners_.add(listener); }
    void RemoveModulationActionListener(ModulationActionListener* listener) { listeners_.remove(listener); }
private:
    Synth& synth_;
    std::unique_ptr<ModulatorsLayout> modulators_layout_;
    std::unique_ptr<MasterLayout> master_;
    std::unique_ptr<juce::TabbedComponent> tabbed_;
    std::vector<std::unique_ptr<ModuContainer>> layouts_;
    juce::ListenerList<ModulationActionListener> listeners_;
};
}
