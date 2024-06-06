#include "spectrum.h"

#include <mutex>
#include "engine/partials.h"
#include "engine/synth.h"

namespace mana {
Spectrum::Spectrum(Synth& synth)
    : synth_(synth) {
    startTimerHz(25);

    box.addItem("auto", 1);
    for (int i = 0; i < 1; ++i) {
        box.addItem(juce::String{ i }, i + 2);
    }
    box.setSelectedItemIndex(0, juce::dontSendNotification);

    addAndMakeVisible(box);
}

void Spectrum::paint(juce::Graphics& g) {
    g.setColour(juce::Colours::white);

    mana::Partials drawing_partials_;

    auto idx = box.getSelectedItemIndex();
    if (idx == 0) {
        drawing_partials_ = synth_.GetDisplayOscillor().GetPartials();
    }
    else {
        drawing_partials_ = synth_.GetOscillor(idx - 1).GetPartials();
    }

    auto bound = getLocalBounds().toFloat();
    for (size_t i = 0; i < mana::kNumPartials; ++i) {
        auto x_nor = drawing_partials_.pitches[i] / 150.0f;
        auto y_nor = 0.0f;
        auto y_gain = std::abs(drawing_partials_.gains[i]);
        if (y_gain > 0.0f) {
            auto y_del = std::clamp(20.0f * std::log10(y_gain), -60.0f, 20.0f);
            y_nor = (y_del + 60.0f) / 80.0f;
        }
        int x = static_cast<int>(bound.getX() + bound.getWidth() * x_nor);
        int y = static_cast<int>(bound.getY() + bound.getHeight() * (1.0f - y_nor));
        g.drawLine(x, y, x, bound.getBottom(), 1.0f);
    }

    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds());
}

void Spectrum::resized() {
    box.setBounds(0, 0, 30, 16);
}

void Spectrum::timerCallback() {
    repaint();
}
}