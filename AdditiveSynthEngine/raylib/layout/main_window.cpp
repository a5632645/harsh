#include "main_window.h"

namespace mana {
MainWindow::MainWindow(Synth & synth)
    : synth_(synth)
    , synth_layout_(synth)
    , effect_layout_(synth)
    , resynthsis_layout_(synth)
    , modulation_matrix_layout_(synth)
    , about_layout_(synth)
    , modulators_layout_(synth)
    , master_(synth) {
    synth_button_.SetText("synth");
    effect_button_.SetText("effect");
    resynthsis_button.SetText("resynthsis");
    matrix_button_.SetText("matrix");
    about_button_.SetText("about");
}

void MainWindow::paint() {
    if (synth_button_.Show()) {
        current_layout_ = Layout::kSynth;
    }
    if (effect_button_.Show()) {
        current_layout_ = Layout::kEffect;
    }
    if (resynthsis_button.Show()) {
        current_layout_ = Layout::kResynthsis;
    }
    if (matrix_button_.Show()) {
        current_layout_ = Layout::kMatrix;
    }
    if (about_button_.Show()) {
        current_layout_ = Layout::kAbout;
    }

    switch (current_layout_) {
    case Layout::kSynth:
        synth_layout_.Paint();
        break;
    case Layout::kEffect:
        effect_layout_.Paint();
        break;
    case Layout::kResynthsis:
        resynthsis_layout_.Paint();
        break;
    case Layout::kMatrix:
        modulation_matrix_layout_.Paint();
        break;
    case Layout::kAbout:
        about_layout_.Paint();
        break;
    default:
        assert(false);
        break;
    }

    modulators_layout_.Paint();
    master_.Paint();
}

void MainWindow::SetBounds(int x, int y, int w, int h) {
    synth_button_.SetBounds(rgc::Bounds(x, y, 100, 16));
    effect_button_.SetBounds(rgc::Bounds(x + 100, y, 100, 16));
    resynthsis_button.SetBounds(rgc::Bounds(x + 200, y, 100, 16));
    matrix_button_.SetBounds(rgc::Bounds(x + 300, y, 100, 16));

    synth_layout_.SetBounds({ float(x), y + 16.0f, float(w), h / 2 - 16.0f });
    effect_layout_.SetBounds(x, y + 16, w, h - 16);
    resynthsis_layout_.SetBounds(float(x), y + 16.0f, float(w), h / 2 - 16.0f);
    modulation_matrix_layout_.SetBounds({ float(x), y + 16.0f, float(w), h / 2 - 16.0f });
    about_layout_.SetBounds(x, y + 16, w, h / 2 - 16);

    constexpr auto kModulatorWidth = 400.0f;
    modulators_layout_.SetBounds({ float(x), y + h - 200.0f , float(w), kModulatorWidth });
    master_.SetBounds(x + kModulatorWidth, y + h - 200, w - kModulatorWidth, 200);
}
}