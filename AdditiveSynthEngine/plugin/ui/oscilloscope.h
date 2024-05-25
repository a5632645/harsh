#pragma once

#include <vector>
#include "engine/forward_decalre.h"
#include <juce_audio_utils/juce_audio_utils.h>

namespace mana {
class Oscilloscope : private juce::AudioVisualiserComponent {
public:
    Oscilloscope(const Synth& synth);
private:
    void paintChannel(juce::Graphics&, juce::Rectangle<float> bounds,
                      const juce::Range<float>* levels, int numLevels, int nextSample) override;

    const Synth& synth_;
};
}