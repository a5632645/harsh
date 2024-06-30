#include "curve_layout.h"

static constexpr auto kQuantizeMapBeginId = 10000;

namespace mana {
class CurveLayout::WrapCurveEditor : public juce::Component {
public:
    WrapCurveEditor() {
        editor_ = std::make_unique<CommonCurveEditor>();
        addAndMakeVisible(editor_.get());

        snap_ = std::make_unique<juce::ToggleButton>("snap");
        snap_->onStateChange = [this] {
            editor_->SetSnapGrid(snap_->getToggleState());
        };
        addAndMakeVisible(snap_.get());

        grid_ = std::make_unique<juce::ToggleButton>("grid");
        grid_->onStateChange = [this] {
            editor_->SetDisplayGrid(grid_->getToggleState());
        };
        grid_->setToggleState(true, juce::NotificationType::sendNotification);
        addAndMakeVisible(grid_.get());

        auto grid_callback = [this] {
            editor_->SetGridNum(xgrid_->getValue(), ygrid_->getValue());
        };
        xgrid_ = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::IncDecButtons, juce::Slider::TextEntryBoxPosition::TextBoxLeft);
        xgrid_->onValueChange = grid_callback;
        xgrid_->setRange(1, 32, 1);
        xgrid_->setValue(8);
        addAndMakeVisible(xgrid_.get());

        ygrid_ = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::IncDecButtons, juce::Slider::TextEntryBoxPosition::TextBoxLeft);
        ygrid_->onValueChange = grid_callback;
        ygrid_->setValue(1);
        ygrid_->setRange(1, 32, 1);
        addAndMakeVisible(ygrid_.get());
    }

    void SetCurve(CurveV2* c) {
        editor_->SetCurve(c);
    }

    void resized() override {
        auto b = getLocalBounds();
        auto up = b.removeFromBottom(20);
        editor_->setBounds(b);
        snap_->setBounds(up.removeFromLeft(20));
        grid_->setBounds(up.removeFromLeft(20));
        xgrid_->setBounds(up.removeFromLeft(up.getWidth()/2));
        ygrid_->setBounds(up);
    }
private:
    std::unique_ptr<juce::ToggleButton> snap_;
    std::unique_ptr<juce::ToggleButton> grid_;
    std::unique_ptr<juce::Slider> xgrid_;
    std::unique_ptr<juce::Slider> ygrid_;
    std::unique_ptr<CommonCurveEditor> editor_;
};
}

namespace mana {
CurveLayout::CurveLayout(CurveBank& bank)
    : curve_bank_(bank) {
    selector_ = std::make_unique<juce::ComboBox>();
    selector_->addListener(this);
    addAndMakeVisible(selector_.get());

    quantize_map_editor_ = std::make_unique<QuantizeMapEditor>();
    curve_editor_ = std::make_unique<WrapCurveEditor>();

    auto quantize_map_ids = bank.GetQuantizeMapIds();
    auto curve_ids = bank.GetCurveIds();
    for (int i = 0; const auto & id : quantize_map_ids) {
        selector_->addItem(juce::String{ id }, kQuantizeMapBeginId + i);
        ++i;
    }

    juce::StringArray sa;
    for (int i = 1; const auto & id : curve_ids) {
        sa.add(juce::String{ id });
    }
    sa.sort(true);
    selector_->addItemList(sa, 1);

    addAndMakeVisible(quantize_map_editor_.get());
    addChildComponent(curve_editor_.get());

    selector_->setSelectedItemIndex(0);
}

CurveLayout::~CurveLayout() = default;

void CurveLayout::resized() {
    constexpr auto kTopSize = 20;
    auto b = getLocalBounds();
    selector_->setBounds(b.removeFromTop(kTopSize).reduced(10, 0));
    quantize_map_editor_->setBounds(b);
    curve_editor_->setBounds(b);
}

void CurveLayout::comboBoxChanged(juce::ComboBox* /*comboBoxThatHasChanged*/) {
    auto id = selector_->getSelectedId();
    auto curve_id = selector_->getItemText(selector_->getSelectedItemIndex());
    if (id >= kQuantizeMapBeginId) {
        quantize_map_editor_->setVisible(true);
        curve_editor_->setVisible(false);
        // todo: change quantize map editor
    }
    else {
        curve_editor_->SetCurve(curve_bank_.GetCurvePtr(curve_id.toStdString()));
        curve_editor_->setVisible(true);
        quantize_map_editor_->setVisible(false);
    }
}
}