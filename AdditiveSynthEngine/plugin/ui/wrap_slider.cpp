#include "wrap_slider.h"

#include "data/juce_param.h"
#include "layout/modulators/modulator_button.h"
#include "engine/synth.h"
#include "layout/main_window.h"

namespace mana {
struct WrapSlider::ParamRefStore {
    virtual ~ParamRefStore() = default;

    virtual bool IsModulatable() = 0;
    virtual std::string_view GetId() = 0;

    template<class T> requires std::derived_from<T, WrapSlider::ParamRefStore>
    T& As() { return *static_cast<T*>(this); }
};

struct FloatParamRefStore : public WrapSlider::ParamRefStore {
    FloatParameter& param_;
    FloatParamRefStore(FloatParameter& param) : param_(param) {}

    bool IsModulatable() override { return param_.GetModulationType() == ModulationType::kPoly; }

    // 通过 ParamRefStore 继承
    std::string_view GetId() override { return param_.GetId(); }
};

struct IntParamRefStore : public WrapSlider::ParamRefStore {
    IntParameter& param_;
    IntParamRefStore(IntParameter& param) : param_(param) {}

    bool IsModulatable() override { return false; }

    // 通过 ParamRefStore 继承
    std::string_view GetId() override { return param_.GetId(); }
};
}

namespace mana {
static class ModuCircleLookAndFeel : public juce::LookAndFeel_V4 {
public:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& s) override {
        juce::Rectangle bound{ x,y,width,height };
        auto current = static_cast<float>(std::numbers::pi_v<float> *2.0f * s.getNormalisableRange().convertTo0to1(s.getValue()));
        auto current_x = x + width / 2.0f * std::cos(current);
        auto current_y = y + height / 2.0f * std::sin(current);

        juce::Path p;
        p.addLineSegment(juce::Line<int>{bound.getCentre(), { bound.getCentreX(), y }}.toFloat(), 1.0f);
        p.addArc(x, y, width, height, 0, current, false);
        p.closeSubPath();
        g.setColour(juce::Colours::grey);
        g.fillPath(p);

        p.clear();
        p.addLineSegment(juce::Line{ bound.getCentre().toFloat(), {current_x, current_y} }, 1.0f);
        p.addArc(x, y, width, height, current, std::numbers::pi_v<float>*2.0f, false);
        p.closeSubPath();
        g.setColour(juce::Colours::black);
        g.fillPath(p);

        g.setColour(juce::Colours::white);
        g.drawEllipse(bound.toFloat(), 1.0f);
    }
} kModuLookAndFeel;
}

namespace mana {
class ModulationCircle : public juce::Component, public ModulationConfig::Listener, public juce::Slider::Listener {
public:
    ModulationCircle(std::shared_ptr<ModulationConfig> config,
                     WrapSlider& parent,
                     Synth& synth)
        : parent_(parent)
        , config_(config)
        , synth_(synth) {
        slider_ = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::RotaryVerticalDrag,
                                                 juce::Slider::TextEntryBoxPosition::NoTextBox);
        slider_->setLookAndFeel(&kModuLookAndFeel);
        slider_->addListener(this);
        slider_->setRange(-1.0f, 1.0f);
        slider_->setPopupDisplayEnabled(true, true, nullptr);
        slider_->textFromValueFunction = [this](double v) -> juce::String {
            auto range = parent_.getNormalisableRange();
            auto parent_curr = range.convertTo0to1(parent_.getValue());
            auto up_nor_v = std::clamp(config_->amount + parent_curr, 0.0, 1.0);
            auto down_nor_v = std::clamp(config_->bipolar ? parent_curr - config_->amount : parent_curr, 0.0, 1.0);
            return juce::String{ std::format("{}: {}=>{}",
                                            config_->modulator_id,
                                            parent_.getTextFromValue(range.convertFrom0to1(down_nor_v)).toStdString(),
                                             parent_.getTextFromValue(range.convertFrom0to1(up_nor_v)).toStdString()) };
        };
        slider_->addMouseListener(this, false);
        addAndMakeVisible(slider_.get());

        popup_menu_ = std::make_unique<juce::PopupMenu>();
        ReCreatePopMenuItems();

        config_->AddListener(this);
    }

    ~ModulationCircle() {
        config_->RemoveListener(this);
        config_ = nullptr;

        slider_->setLookAndFeel(nullptr);
        slider_->removeListener(this);
        slider_ = nullptr;
    }

    void resized() override {
        slider_->setBounds(getLocalBounds());
    }

    void mouseDown(const juce::MouseEvent& event) override {
        if (event.mods.isPopupMenu()) {
            popup_menu_->showMenuAsync(juce::PopupMenu::Options{});
        }
    }

    ModulationConfig& GetConfig() { return *config_; }
private:
    void ReCreatePopMenuItems() {
        popup_menu_->clear();
        popup_menu_->addItem(config_->bipolar ? "unmake bipolar" : "make bipolar", [this]() {
            config_->SetBipolar(!config_->bipolar);
            ReCreatePopMenuItems();
        });
        popup_menu_->addItem(config_->enable ? "disable" : "enable", [this]() {
            config_->SetEnable(!config_->enable);
            ReCreatePopMenuItems();
        });
        popup_menu_->addItem("remove", [this]() {
            synth_.RemoveModulation(*config_);
        });
    }

    WrapSlider& parent_;
    Synth& synth_;
    std::shared_ptr<ModulationConfig> config_;
    std::unique_ptr<juce::Slider> slider_;
    std::unique_ptr<juce::PopupMenu> popup_menu_;

    // 通过 Listener 继承
    void OnConfigChanged(ModulationConfig* config) override {
        if (config != config_.get()) return;
        slider_->setValue(config->amount);
        ReCreatePopMenuItems();
    }

    // 通过 Listener 继承
    void sliderValueChanged(juce::Slider* slider) override {
        config_->SetAmount(slider->getValue());
    }
};
}

namespace mana {
class ModulationTab : public juce::Component, public juce::Timer {
public:
    ModulationTab(Synth& synth, WrapSlider& parent)
        : synth_(synth)
        , parent_(parent) {
        startTimer(1000);
    }

    void resized() override {
        for (int i = 0; const auto & pcircle : circles_) {
            pcircle->setBounds(i * kCircleSize, 0, kCircleSize, kCircleSize);
            ++i;
        }
    }

    void AddModulation(std::shared_ptr<ModulationConfig> config) {
        const auto& pc = circles_.emplace_back(std::make_unique<ModulationCircle>(config, parent_, synth_));
        addAndMakeVisible(pc.get());

        ReCalcSize();
    }

    void RemoveModulation(std::string_view modulator_id) {
        auto it = std::ranges::find_if(circles_,
                                       [modulator_id](const auto& p) -> bool {
            return p->GetConfig().modulator_id == modulator_id;
        });

        if (it != circles_.end()) {
            removeChildComponent(it->get());
            circles_.erase(it);

            ReCalcSize();
        }
    }

    void MarkHide() {
        should_hide_ = true;
    }
private:
    static constexpr auto kCircleSize = 20;

    int GetNumCircles() const { return static_cast<int>(circles_.size()); }
    void ReCalcSize() {
        setSize(kCircleSize * GetNumCircles(), kCircleSize);
    }

    Synth& synth_;
    WrapSlider& parent_;
    std::vector<std::unique_ptr<ModulationCircle>> circles_;
    bool should_hide_{};

    // 通过 Timer 继承
    void timerCallback() override {
        if (should_hide_) {
            // check mouse is in this component
            if (!isMouseOverOrDragging(true)) {
                setVisible(false);
                should_hide_ = false;
            }
        }
    }
};
}

namespace mana {
WrapSlider::WrapSlider(FloatParameter* p)
    : juce::Slider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox)
    , parameter_(p->As<JuceFloatParam>().GetRef()) {
    attachment_ = std::make_unique<juce::SliderParameterAttachment>(p->As<JuceFloatParam>().GetRef(), *this);
    ref_store_ = std::make_unique<FloatParamRefStore>(*p);
    setPopupDisplayEnabled(true, true, nullptr);
}

WrapSlider::WrapSlider(IntParameter * p)
    : juce::Slider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox)
    , parameter_(p->As<JuceIntParam>().GetRef()) {
    attachment_ = std::make_unique<juce::SliderParameterAttachment>(p->As<JuceIntParam>().GetRef(), *this);
    ref_store_ = std::make_unique<IntParamRefStore>(*p);
    setPopupDisplayEnabled(true, true, nullptr);
}

WrapSlider::~WrapSlider() {
    if (synth_ != nullptr) {
        synth_->GetSynthParams().RemoveModulationListener(this);
    }

    attachment_ = nullptr;
    ref_store_ = nullptr;
    synth_ = nullptr;
    modulation_tab_ = nullptr;
}

void WrapSlider::paint(juce::Graphics& g) {
    if (shound_highlight_) {
        g.fillAll(juce::Colours::aqua);
    }
    juce::Slider::paint(g);
}

void WrapSlider::resized() {
    juce::Slider::resized();

    auto* main_window = findParentComponentOfClass<MainWindow>();
    if (main_window == nullptr)
        return;

    if (synth_ == nullptr) {
        synth_ = &main_window->GetSynth();
        synth_->GetSynthParams().AddModulationListener(this);
        main_window->AddModulationActionListener(this);

        modulation_tab_ = std::make_unique<ModulationTab>(*synth_, *this);
        main_window->addChildComponent(modulation_tab_.get());
    }

    if (modulation_tab_ != nullptr) {
        auto slider_in_main_window = main_window->getLocalPoint(this, getLocalBounds().getBottomLeft());
        modulation_tab_->setTopLeftPosition(slider_in_main_window);
    }
}

void WrapSlider::BeginHighlightModulator(std::string_view id) {
    if (CanBeModulateBy(id)) {
        shound_highlight_ = true;
        repaint();
    }
}

void WrapSlider::StopHighliteModulator() {
    shound_highlight_ = false;
    repaint();
}

void WrapSlider::mouseEnter(const juce::MouseEvent& event) {
    juce::Slider::mouseEnter(event);

    if (modulation_tab_ == nullptr)
        return;

    modulation_tab_->setVisible(true);
}

void WrapSlider::mouseExit(const juce::MouseEvent& event) {
    juce::Slider::mouseExit(event);

    if (modulation_tab_ == nullptr)
        return;

    modulation_tab_->MarkHide();
}

bool WrapSlider::CanBeModulateBy(std::string_view modulator_id) {
    if (!ref_store_->IsModulatable())
        return false;

    if (synth_ == nullptr)
        return true;

    auto& storer = ref_store_->As<FloatParamRefStore>();
    auto id = storer.param_.GetId();
    return synth_->GetSynthParams().FindModulation(modulator_id, id) == nullptr;
}

bool WrapSlider::isInterestedInDragSource(const SourceDetails& dragSourceDetails) {
    return CanBeModulateBy(dragSourceDetails.description.toString().toStdString());
}

void WrapSlider::itemDropped(const SourceDetails& dragSourceDetails) {
    auto modulator_id = dragSourceDetails.description.toString().toStdString();
    auto source_button_flag_component_ptr = dragSourceDetails.sourceComponent;
    if (source_button_flag_component_ptr.wasObjectDeleted()) {
        return;
    }

    auto param_id = parameter_.paramID.toStdString();
    synth_->CreateModulation(modulator_id, param_id);
}

void WrapSlider::OnModulationAdded(std::shared_ptr<ModulationConfig> config) {
    if (config->param_id == ref_store_->GetId())
        modulation_tab_->AddModulation(config);
}

void WrapSlider::OnModulationRemoved(std::string_view modulator_id, std::string_view param_id) {
    if (param_id == ref_store_->GetId())
        modulation_tab_->RemoveModulation(modulator_id);
}
}