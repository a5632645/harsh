#pragma once

#include "engine/forward_decalre.h"

#include "modulators/modulators_layout.h"
#include "master_layout.h"

namespace mana {
class MainWindow : public juce::Component {
public:
    MainWindow(Synth& synth);

    void resized() override;
    //Oscilloscope& GetWaveScope() { return master_.GetWaveScope(); }
private:
    Synth& synth_;
    ModulatorsLayout modulators_layout_;
    MasterLayout master_;
    std::unique_ptr<juce::TabbedComponent> tabbed_;
    std::vector<std::unique_ptr<ModuContainer>> layouts_;
};
}
