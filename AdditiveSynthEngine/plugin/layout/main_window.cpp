#include "main_window.h"

#include "synth/synth_layout.h"
#include "matrix/modulation_matrix_layout.h"
#include "effect/final_fx_layout.h"
//#include "about/about_layout.h"
#include "resynthsis/resynthsis_layout.h"
#include "engine/synth.h"

namespace mana {
MainWindow::MainWindow(Synth & synth)
    : synth_(synth) {
    modulators_layout_ = std::make_unique<ModulatorsLayout>(synth);
    master_ = std::make_unique<MasterLayout>(synth);
    tabbed_ = std::make_unique<juce::TabbedComponent>(juce::TabbedButtonBar::Orientation::TabsAtTop);

    tabbed_->addTab("Synth", juce::Colours::darkgrey, new SynthLayout(synth), true);
    tabbed_->addTab("Effect", juce::Colours::darkgrey, new FinalFxLayout(synth), true);
    tabbed_->addTab("Resynthsis", juce::Colours::darkgrey, new ResynthsisLayout(synth), true);
    tabbed_->addTab("Modulation Matrix", juce::Colours::darkgrey, new ModulationMatrixLayout(synth), true);

    addAndMakeVisible(*tabbed_);
    addAndMakeVisible(*modulators_layout_);
    addChildComponent(*master_);

    curve_layout_ = std::make_unique<CurveLayout>(synth.GetSynthParams().GetCurveBank());
    addAndMakeVisible(curve_layout_.get());
}

void MainWindow::resized() {
    auto bound = getLocalBounds();
    auto bottom_bound = bound.removeFromBottom(200);
    tabbed_->setBounds(bound);

    constexpr auto kModulatorWidth = 400.0f;
    auto modulator_bound = bottom_bound.removeFromLeft(kModulatorWidth);
    modulators_layout_->setBounds(modulator_bound);
    master_->setBounds(bottom_bound);
    curve_layout_->setBounds(bottom_bound);
}

void MainWindow::BeginHighlightModulator(std::string_view id) {
    listeners_.call(&ModulationActionListener::BeginHighlightModulator, id);
}

void MainWindow::StopHighliteModulator() {
    listeners_.call(&ModulationActionListener::StopHighliteModulator);
}
}