#pragma once

#include <vector>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_events/juce_events.h>
#include "engine/forward_decalre.h"

namespace mana {
class Spectrum
    : public juce::Component
    , private juce::Timer {
public:
    Spectrum(Synth& synth) : synth_(synth) { startTimerHz(25); }
    ~Spectrum() override { stopTimer(); }

    void paint(juce::Graphics& g) override;
private:
    void timerCallback() override;

    Synth& synth_;
};
}