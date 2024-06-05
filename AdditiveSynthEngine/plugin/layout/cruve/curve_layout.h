#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "ui/common_curve_editor.h"
#include "ui/quantize_map_editor.h"

#include "engine/modulation/curve_bank.h"

namespace mana {
class CurveLayout : public juce::Component, public juce::ComboBox::Listener {
public:
    CurveLayout(CurveBank& bank);

    void resized() override;
private:
    CurveBank& curve_bank_;
    std::unique_ptr<CommonCurveEditor> curve_editor_;
    std::unique_ptr<QuantizeMapEditor> quantize_map_editor_;
    std::unique_ptr<juce::ComboBox> selector_;

    // 通过 Listener 继承
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
};
}