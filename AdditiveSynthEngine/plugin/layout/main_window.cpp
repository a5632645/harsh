#include "main_window.h"

#include "synth/synth_layout.h"
#include "matrix/modulation_matrix_layout.h"
#include "effect/final_fx_layout.h"
//#include "about/about_layout.h"
#include "resynthsis/resynthsis_layout.h"

namespace mana {
MainWindow::MainWindow(Synth & synth)
    : synth_(synth) {
    modulators_layout_ = std::make_unique<ModulatorsLayout>(synth);
    master_ = std::make_unique<MasterLayout>(synth);
    tabbed_ = std::make_unique<juce::TabbedComponent>(juce::TabbedButtonBar::Orientation::TabsAtTop);

    auto synth_layout = std::make_unique<SynthLayout>(synth);
    auto effect_layout = std::make_unique<FinalFxLayout>(synth);
    auto resynthsis_layout = std::make_unique<ResynthsisLayout>(synth);
    //auto about_layout = std::make_unique<AboutLayout>(synth);

    tabbed_->addTab("Synth", juce::Colours::darkgrey, synth_layout.get(), false);
    tabbed_->addTab("Effect", juce::Colours::darkgrey, effect_layout.get(), false);
    tabbed_->addTab("Resynthsis", juce::Colours::darkgrey, resynthsis_layout.get(), false);
    tabbed_->addTab("Modulation Matrix", juce::Colours::darkgrey, new ModulationMatrixLayout(synth), true);
    //tabbed_->addTab("About", juce::Colours::green, about_layout.get(), false);

    layouts_.emplace_back(std::move(synth_layout));
    layouts_.emplace_back(std::move(effect_layout));
    layouts_.emplace_back(std::move(resynthsis_layout));
    //layouts_.emplace_back(std::move(about_layout));

    addAndMakeVisible(*tabbed_);
    addAndMakeVisible(*modulators_layout_);
    addAndMakeVisible(*master_);
}

void MainWindow::resized() {
    //synth_button_.SetBounds(rgc::Bounds(x, y, 100, 16));
    //effect_button_.SetBounds(rgc::Bounds(x + 100, y, 100, 16));
    //resynthsis_button.SetBounds(rgc::Bounds(x + 200, y, 100, 16));
    //matrix_button_.SetBounds(rgc::Bounds(x + 300, y, 100, 16));

    //synth_layout_.SetBounds({ float(x), y + 16.0f, float(w), h / 2 - 16.0f });
    //effect_layout_.SetBounds(x, y + 16, w, h - 16);
    //resynthsis_layout_.SetBounds(float(x), y + 16.0f, float(w), h / 2 - 16.0f);
    //modulation_matrix_layout_.SetBounds({ float(x), y + 16.0f, float(w), h / 2 - 16.0f });
    //about_layout_.SetBounds(x, y + 16, w, h / 2 - 16);

    auto bound = getLocalBounds();
    auto bottom_bound = bound.removeFromBottom(200);
    tabbed_->setBounds(bound);

    constexpr auto kModulatorWidth = 400.0f;
    auto modulator_bound = bottom_bound.removeFromLeft(kModulatorWidth);
    modulators_layout_->setBounds(modulator_bound);
    master_->setBounds(bottom_bound);
}

void MainWindow::BeginHighlightModulator(std::string_view id) {
    listeners_.call(&ModulationActionListener::BeginHighlightModulator, id);
}

void MainWindow::StopHighliteModulator() {
    listeners_.call(&ModulationActionListener::StopHighliteModulator);
}
}