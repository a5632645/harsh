#pragma once

#include "engine/forward_decalre.h"

#include "synth/synth_layout.h"
#include "matrix/modulation_matrix_layout.h"
#include "effect/final_fx_layout.h"
#include "about/about_layout.h"
#include "resynthsis/resynthsis_layout.h"
#include "modulators/modulators_layout.h"
#include "master_layout.h"

namespace mana {
class MainWindow {
public:
    MainWindow(Synth& synth);

    void paint();
    void SetBounds(int x, int y, int w, int h);

    Oscilloscope& GetWaveScope() { return master_.GetWaveScope(); }
private:
    enum class Layout {
        kSynth,
        kEffect,
        kResynthsis,
        kMatrix,
        kAbout
    };

    Synth& synth_;
    ModulatorsLayout modulators_layout_;
    MasterLayout master_;

    SynthLayout synth_layout_;
    ModulationMatrixLayout modulation_matrix_layout_;
    FinalFxLayout effect_layout_;
    AboutLayout about_layout_;
    ResynthsisLayout resynthsis_layout_;

    // swich page
    Layout current_layout_{ Layout::kSynth };
    rgc::Button synth_button_;
    rgc::Button effect_button_;
    rgc::Button resynthsis_button;
    rgc::Button matrix_button_;
    rgc::Button about_button_;
};
}
