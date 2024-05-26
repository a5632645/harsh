#include "modulation_matrix_layout.h"

#include "engine/modulation/Modulation.h"
#include "engine/synth.h"

namespace mana {
class ModuWrapToggleBox : public juce::ToggleButton {
public:
    ModuWrapToggleBox(ModulationConfig* cfg) {
        this->onClick = [this]() {
            if (config_ == nullptr) return;
            config_->enable = this->getToggleState();
        };

        SetModulationConfig(cfg);
    }

    void SetModulationConfig(ModulationConfig* cfg) {
        setToggleState(cfg->enable, juce::dontSendNotification);
        config_ = cfg;
    }
private:
    ModulationConfig* config_;
};

class ModuWrapAmountSlider : public juce::Slider {
public:
    ModuWrapAmountSlider(ModulationConfig* cfg) {
        this->onValueChange = [this]() {
            if (config_ == nullptr) return;
            config_->amount = this->getValue();
        };

        SetModulationConfig(cfg);
    }

    void SetModulationConfig(ModulationConfig* cfg) {
        setRange(-1.0f, 1.0f);
        setDoubleClickReturnValue(true, 0.0);
        setValue(cfg->amount);
        config_ = cfg;
    }

private:
    ModulationConfig* config_;
};

class ModuWrapBipolarToggleBox : public juce::ToggleButton {
public:
    ModuWrapBipolarToggleBox(ModulationConfig* cfg) {
        this->onClick = [this]() {
            if (config_ == nullptr) return;
            config_->bipolar = this->getToggleState();
        };

        SetModulationConfig(cfg);
    }

    void SetModulationConfig(ModulationConfig* cfg) {
        setToggleState(cfg->bipolar, juce::dontSendNotification);
        config_ = cfg;
    }

private:
    ModulationConfig* config_;
};

class ModuWrapRemoveButton : public juce::TextButton {
public:
    ModuWrapRemoveButton(ModulationConfig* cfg) : config_(cfg), TextButton("remove") {}

    ModulationConfig* GetModulationConfig() { return config_; }
    void SetModulationConfig(ModulationConfig* cfg) { config_ = cfg; }
private:
    ModulationConfig* config_;
};

//class ModulationMatrixLayout::SingleModulationLayout : public juce::Component {
//public:
//    SingleModulationLayout(ModulationConfig& cfg) : config_ref_(cfg) {
//        amount_.set_range(-1.0f, 1.0f, 0.0025f, 0.5f);
//        amount_.set_title("amount");
//        enable_.SetText("enable");
//        bipolar_.SetText("bipolar");
//        remove_.SetText("remove");
//    }
//
//    void resized() override {
//        enable_.SetBounds(rgc::Bounds(bound.x, bound.y, 16, 16));
//        bipolar_.SetBounds(rgc::Bounds(bound.x + 100, bound.y, 16, 16));
//        amount_.set_bound({ bound.x + 150,bound.y,50,50 });
//        remove_.SetBounds(rgc::Bounds(bound.x + 200, bound.y, 50, 16));
//    }
//
//    ModulationConfig& GetModulationConfigRef() { return config_ref_; }
//
//    std::function<void(SingleModulationLayout*)> on_remove_click = [](auto) {};
//private:
//    ModulationConfig& config_ref_;
//    std::unique_ptr<juce::ToggleButton> enable_;
//    std::unique_ptr<juce::ToggleButton> bipolar_;
//    std::unique_ptr<juce::Slider> amount_;
//    std::unique_ptr<juce::TextButton> remove_;
//};

ModulationMatrixLayout::ModulationMatrixLayout(Synth& synth)
    : synth_(synth) {
    param_selector_ = std::make_unique<juce::ComboBox>("param_selector");
    section_selector_ = std::make_unique<juce::ComboBox>("section_selector");
    modulator_selector_ = std::make_unique<juce::ComboBox>("modulator_selector");
    add_button_ = std::make_unique<juce::TextButton>("add");
    add_button_->addListener(this);
    table_ = std::make_unique<juce::TableListBox>("table", this);

    table_->getHeader().addColumn("modulator", 1, 100);
    table_->getHeader().addColumn("param", 2, 100);
    table_->getHeader().addColumn("enable", 3, 100);
    table_->getHeader().addColumn("bipolar", 4, 100);
    table_->getHeader().addColumn("amount", 5, 250);
    table_->getHeader().addColumn("remove", 6, 100);

    addAndMakeVisible(*param_selector_);
    addAndMakeVisible(*section_selector_);
    addAndMakeVisible(*modulator_selector_);
    addAndMakeVisible(*add_button_);
    addAndMakeVisible(*table_);

    // process param ids
    modulator_ids_ = synth.GetModulatorIds();
    auto param_ids = synth.GetModulableParamIds();

    // split param ids
    for (std::string_view id : param_ids) {
        auto sp_idx = id.find_first_of('.');
        auto section = id.substr(0, sp_idx);
        auto detail = id.substr(sp_idx + 1);
        split_param_ids[section].emplace_back(detail, id);
    }
    auto section_view = split_param_ids | std::ranges::views::keys;
    sections_.assign(section_view.begin(), section_view.end());

    {
        juce::StringArray sa;
        for (auto sv : section_view) {
            sa.add(juce::String{ sv.data(), sv.length() });
        }
        section_selector_->addItemList(sa, 1);
        section_selector_->addListener(this);
    }
    section_selector_->setSelectedItemIndex(0, juce::sendNotification);

    {
        juce::StringArray sa;
        for (auto id : modulator_ids_) {
            sa.add(juce::String{ id.data(), id.length() });
        }
        modulator_selector_->addItemList(sa, 1);
        modulator_selector_->addListener(this);
    }
    modulator_selector_->setSelectedItemIndex(0, juce::sendNotification);
}

ModulationMatrixLayout::~ModulationMatrixLayout() = default;

void ModulationMatrixLayout::resized() {
    constexpr int kTopSize = 24;
    section_selector_->setBounds(0, 0, 100, kTopSize);
    param_selector_->setBounds(0 + 100, 0, 100, kTopSize);
    modulator_selector_->setBounds(0 + 200, 0, 100, kTopSize);
    add_button_->setBounds(0 + 300, 0, 100, kTopSize);

    auto b = getLocalBounds();
    b.removeFromTop(kTopSize);
    table_->setBounds(b);
}

void ModulationMatrixLayout::OnAddClick() {
    auto modulator_idx = modulator_selector_->getSelectedItemIndex();
    auto param_idx = param_selector_->getSelectedItemIndex();
    auto modulator_id = modulator_ids_[modulator_idx];
    auto section = sections_[section_selector_->getSelectedItemIndex()];
    auto param_id = split_param_ids[section][param_idx].id;

    auto&&[added, pconfig] = synth_.CreateModulation(modulator_id, param_id);
    if (!added) {
        return;
    }

    table_->updateContent();
}

int ModulationMatrixLayout::getNumRows() {
    return synth_.GetModulatorCount();
}

void ModulationMatrixLayout::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) {
    auto alternateColour = getLookAndFeel().findColour(juce::ListBox::backgroundColourId)
        .interpolatedWith(getLookAndFeel().findColour(juce::ListBox::textColourId), 0.03f);
    if (rowIsSelected)
        g.fillAll(juce::Colours::lightblue);
    else if (rowNumber % 2)
        g.fillAll(alternateColour);
}

void ModulationMatrixLayout::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) {
    if (columnId != 1 && columnId != 2) {
        return;
    }

    if (rowNumber >= synth_.GetModulatorCount()) {
        return;
    }

    auto config = synth_.GetModulationConfig(rowNumber);
    g.setColour(getLookAndFeel().findColour(juce::ListBox::textColourId));
    if (columnId == 1) {
        auto modulator_id = config->modulator_id;
        g.drawText(juce::String{ modulator_id.data(), modulator_id.length() }, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
    }
    else if (columnId == 2) {
        auto param_id = config->param_id;
        g.drawText(juce::String{ param_id.data(), param_id.length() }, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
    }

    g.setColour(getLookAndFeel().findColour(juce::ListBox::backgroundColourId));
    g.fillRect(width - 1, 0, 1, height);
}

juce::Component* ModulationMatrixLayout::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, juce::Component* existingComponentToUpdate) {
    if (rowNumber >= synth_.GetModulatorCount()) {
        return nullptr;
    }

    auto config = synth_.GetModulationConfig(rowNumber).get();
    if (columnId == 3) { // enable
        if (existingComponentToUpdate == nullptr) {
            return new ModuWrapToggleBox(config);
        }
        else {
            static_cast<ModuWrapToggleBox*>(existingComponentToUpdate)->SetModulationConfig(config);
            return existingComponentToUpdate;
        }
    }
    else if (columnId == 4) { // bipolar
        if (existingComponentToUpdate == nullptr) {
            return new ModuWrapBipolarToggleBox(config);
        }
        else {
            static_cast<ModuWrapBipolarToggleBox*>(existingComponentToUpdate)->SetModulationConfig(config);
            return existingComponentToUpdate;
        }
    }
    else if (columnId == 5) { // amount
        if (existingComponentToUpdate == nullptr) {
            return new ModuWrapAmountSlider(config);
        }
        else {
            static_cast<ModuWrapAmountSlider*>(existingComponentToUpdate)->SetModulationConfig(config);
            return existingComponentToUpdate;
        }
    }
    else if (columnId == 6) { // remove
        if (existingComponentToUpdate == nullptr) {
            auto p = new ModuWrapRemoveButton(config);
            p->addListener(this);
            return p;
        }
        else {
            static_cast<ModuWrapRemoveButton*>(existingComponentToUpdate)->SetModulationConfig(config);
            return existingComponentToUpdate;
        }
    }

    return nullptr;
}

void ModulationMatrixLayout::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) {
    if (comboBoxThatHasChanged == section_selector_.get()) {
        int current_section_idx = comboBoxThatHasChanged->getSelectedItemIndex();
        auto param_ids = split_param_ids[sections_[current_section_idx]] | std::views::transform([](SplitParamId& id) {
            return id.detail;
        });

        juce::StringArray sa;
        for (auto id : param_ids) {
            sa.add(juce::String{ id.data(), id.length() });
        }
        param_selector_->clear(juce::dontSendNotification);
        param_selector_->addItemList(sa, 1);
        param_selector_->setSelectedItemIndex(0, juce::dontSendNotification);
    }
}

void ModulationMatrixLayout::buttonClicked(juce::Button* ptr_button) {
    if (ptr_button == add_button_.get()) {
        OnAddClick();
        return;
    }

    auto* remove_button = static_cast<ModuWrapRemoveButton*>(ptr_button);
    auto config = remove_button->GetModulationConfig();
    synth_.RemoveModulation(*config);
    table_->updateContent();
}
}