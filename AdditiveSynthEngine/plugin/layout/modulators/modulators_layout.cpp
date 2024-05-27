#include "modulators_layout.h"

#include "engine/synth.h"
#include "lfo_layout.h"
#include "envelop_layout.h"
#include "modulator_button.h"

namespace mana {
ModulatorsLayout::ModulatorsLayout(Synth& synth)
    : tabbed_(juce::TabbedButtonBar::Orientation::TabsAtTop) {
    const auto ids = synth.GetModulatorIds();
    for (int tab_idx = 0; auto id : ids) {
        if (id.starts_with("lfo")) {
            auto idx_str = id.substr(3);
            int d{};
            auto [ptr, err] = std::from_chars(idx_str.cbegin()._Unwrapped(), idx_str.cend()._Unwrapped(), d);
            assert(err == std::error_code{});

            auto comp = std::make_unique<LfoLayout>(synth, d);
            tabbed_.addTab(juce::String{ id.data(), id.length() }, juce::Colours::green, comp.get(), false);
            components_.emplace_back(std::move(comp));
        }
        else if (id.starts_with("env")) {
            auto idx_str = id.substr(3);
            int d{};
            auto [ptr, err] = std::from_chars(idx_str.cbegin()._Unwrapped(), idx_str.cend()._Unwrapped(), d);
            assert(err == std::error_code{});

            auto comp = std::make_unique<EnvelopLayout>(synth, d);
            tabbed_.addTab(juce::String{ id.data(), id.length() }, juce::Colours::green, comp.get(), false);
            components_.emplace_back(std::move(comp));
        }

        // extra num modulation display and drag component
        tabbed_.getTabbedButtonBar().getTabButton(tab_idx)->setExtraComponent(new ModulatorButton(id, synth),
                                                                              juce::TabBarButton::ExtraComponentPlacement::afterText);
        ++tab_idx;
    }

    addAndMakeVisible(tabbed_);
}

void ModulatorsLayout::resized() {
    tabbed_.setBounds(getLocalBounds());
}

void ModulatorsLayout::BeginHighlightModulator(std::string_view id) {
    for (const auto& c : components_) {
        c->BeginHighlightModulator(id);
    }
}

void ModulatorsLayout::StopHighliteModulator() {
    for (const auto& c : components_) {
        c->StopHighliteModulator();
    }
}
}