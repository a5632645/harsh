#include "main_window.h"

namespace mana {
MainWindow::MainWindow(Synth & synth)
    : synth_(synth)
    , synth_layout_(synth)
    , curve_layout_(synth.GetCurveManager())
    , modulator_layout_(synth)
    , modulation_matrix_layout_(synth) {
    synth_button_.SetText("synth");
    curve_button_.SetText("curve");
    modulator_button_.SetText("modulator");
    modulation_matrix_button_.SetText("matrix");
}

void MainWindow::paint() {
    if (synth_button_.Show()) {
        current_layout_ = Layout::kSynth;
    }
    if (curve_button_.Show()) {
        current_layout_ = Layout::kCurve;
    }
    if (modulator_button_.Show()) {
        current_layout_ = Layout::kModulators;
    }
    if (modulation_matrix_button_.Show()) {
        current_layout_ = Layout::kMatrix;
    }

    switch (current_layout_) {
    case Layout::kSynth:
        synth_layout_.paint();
        break;
    case Layout::kCurve:
        curve_layout_.Paint();
        break;
    case Layout::kModulators:
        modulator_layout_.Paint();
        break;
    case Layout::kMatrix:
        modulation_matrix_layout_.Paint();
        break;
    default:
        break;
    }
}

void MainWindow::SetBounds(int x, int y, int w, int h) {
    synth_button_.SetBounds(rgc::Bounds(x, y, 100, 16));
    curve_button_.SetBounds(rgc::Bounds(x + 100, y, 100, 16));
    modulator_button_.SetBounds(rgc::Bounds(x + 200, y, 100, 16));
    modulation_matrix_button_.SetBounds(rgc::Bounds(x + 300, y, 100, 16));

    synth_layout_.SetBounds(x, y + 16, w, h - 16);
    curve_layout_.SetBounds(x, y + 16, w, h - 16);
    modulator_layout_.SetBounds({ float(x), y + 16.0f, float(w), h - 16.0f });
    modulation_matrix_layout_.SetBounds({ float(x), y + 16.0f, float(w), h - 16.0f });
}
}