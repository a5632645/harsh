#include "spectrum.h"

#include <mutex>
#include "engine/partials.h"
#include "engine/synth.h"
#include <utli/convert.h>

namespace mana {
Spectrum::Spectrum(Synth& synth)
    : synth_(synth) {
    startTimerHz(25);
}

void Spectrum::paint(juce::Graphics& g) {
    g.setColour(juce::Colours::white);

    constexpr auto min_pitch = utli::cp::FreqToPitch(20.0f);
    constexpr auto max_pitch = utli::cp::FreqToPitch(20000.0f);
    constexpr auto min_db = -60.0f;
    constexpr auto max_db = 20.0f;

    const auto& synth_partials = synth_.GetDisplayOscillor().GetPartials();
    auto gains = synth_partials.gains;
    auto pitches = synth_partials.pitches;

    auto bound = getLocalBounds().toFloat();
    for (size_t i = 0; i < mana::kNumPartials; ++i) {
        if (pitches[i] < min_pitch || pitches[i] > max_pitch)
            continue;

        auto x_nor = (pitches[i] - min_pitch) / (max_pitch - min_pitch);
        auto y_db = utli::GainToDb<min_db>(std::abs(gains[i]));
        auto y_nor = (y_db - min_db) / (max_db - min_db);
        int x = static_cast<int>(bound.getX() + bound.getWidth() * x_nor);
        int y = static_cast<int>(bound.getY() + bound.getHeight() * (1.0f - y_nor));
        g.drawLine(x, y, x, bound.getBottom(), 1.0f);
    }

    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds());
}

void Spectrum::timerCallback() {
    repaint();
}
}