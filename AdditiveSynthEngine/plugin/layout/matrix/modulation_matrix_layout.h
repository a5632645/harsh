#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <juce_gui_basics/juce_gui_basics.h>
#include "engine/forward_decalre.h"
#include "engine/synth_params.h"

namespace mana {
class ModulationMatrixLayout : public juce::Component
    , public juce::TableListBoxModel
    , private juce::ComboBox::Listener
    , public juce::Button::Listener
    , public SynthParams::ModulationListener {
public:
    ModulationMatrixLayout(Synth& synth);
    ~ModulationMatrixLayout();

    void resized() override;
private:
    Synth& synth_;
    std::unique_ptr<juce::ComboBox> param_selector_;
    std::unique_ptr<juce::ComboBox> section_selector_;
    std::unique_ptr<juce::ComboBox> modulator_selector_;
    std::unique_ptr<juce::TextButton> add_button_;
    std::unique_ptr<juce::TableListBox> table_;

    void OnAddClick();
    std::vector<std::string_view> modulator_ids_;
    std::vector<std::string_view> sections_;

    struct SplitParamId {
        std::string_view detail;
        std::string_view id;
    };
    std::unordered_map<std::string_view, std::vector<SplitParamId>> split_param_ids;

    // 通过 TableListBoxModel 继承
    int getNumRows() override;
    void paintRowBackground(juce::Graphics&, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(juce::Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    juce::Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, juce::Component* existingComponentToUpdate) override;

    // 通过 Listener 继承
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

    // 通过 Listener 继承
    void buttonClicked(juce::Button*) override;

    // 通过 ModulationListener 继承
    void OnModulationAdded(std::shared_ptr<ModulationConfig> config) override;
    void OnModulationRemoved(std::string_view modulator_id, std::string_view param_id) override;
    void OnModulationCleared() override;
};
}