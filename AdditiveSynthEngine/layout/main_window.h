#pragma once

#include "engine/forward_decalre.h"
#include "ui/oscilloscope.h"
#include "layout/SynthLayout.h"
#include "layout/curve_layout.h"
#include "layout/modulator_layout.h"
#include "layout/modulation_matrix_layout.h"

namespace mana {
class MainWindow {
public:
    MainWindow(Synth& synth);

    void paint();
    void SetBounds(int x, int y, int w, int h);
    Oscilloscope& GetOscilloscope() { return synth_layout_.GetOscilloscope(); }
private:
    enum class Layout {
        kSynth,
        kCurve,
        kModulators,
        kMatrix
    };

    Synth& synth_;

    SynthLayout synth_layout_;
    CurveLayout curve_layout_;
    ModulatorLayout modulator_layout_;
    ModulationMatrixLayout modulation_matrix_layout_;

    // swich page
    Layout current_layout_{ Layout::kSynth };
    rgc::Button synth_button_;
    rgc::Button curve_button_;
    rgc::Button modulator_button_;
    rgc::Button modulation_matrix_button_;
};
}
