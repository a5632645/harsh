#include "curve_layout.h"

static constexpr auto kQuantizeMapBeginId = 10000;

namespace mana {
CurveLayout::CurveLayout(CurveBank& bank)
    : curve_bank_(bank) {
    selector_ = std::make_unique<juce::ComboBox>();
    selector_->addListener(this);
    addAndMakeVisible(selector_.get());

    quantize_map_editor_ = std::make_unique<QuantizeMapEditor>();
    curve_editor_ = std::make_unique<CommonCurveEditor>();

    auto quantize_map_ids = bank.GetQuantizeMapIds();
    auto curve_ids = bank.GetCurveIds();
    for (int i = 0; const auto & id : quantize_map_ids) {
        selector_->addItem(juce::String{ id }, kQuantizeMapBeginId + i);
        ++i;
    }
    for (int i = 1; const auto & id : curve_ids) {
        selector_->addItem(juce::String{ id }, i);
        ++i;
    }

    addAndMakeVisible(quantize_map_editor_.get());
    addChildComponent(curve_editor_.get());

    selector_->setSelectedItemIndex(0);
}

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