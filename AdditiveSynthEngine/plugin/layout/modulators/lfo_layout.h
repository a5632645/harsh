#pragma once

#include <array>
#include "engine/forward_decalre.h"
#include "ui/Knob.h"
#include "ui/WrapDropBox.h"
#include "ui/wrap_check_box.h"
#include "layout_base.h"

namespace mana {
class LfoLayout : public LayoutBase {
public:
    LfoLayout(Synth& synth, int idx, std::string_view id);
    void Paint() override;
    void SetBounds(Rectangle bound) override;
private:
    WrapSlider rate_;
    WrapSlider start_phase_;
    WrapSlider level_;
    WrapCheckBox restart_;
    WrapDropBox wave_type_;
};
}