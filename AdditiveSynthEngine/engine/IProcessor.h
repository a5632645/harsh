#pragma once

#include "partials.h"
#include "param/synth_param.h"

namespace mana {
class IProcessor {
public:
    IProcessor() = default;
    virtual ~IProcessor() = default;

    IProcessor(const IProcessor&) = default;
    IProcessor& operator=(const IProcessor&) = default;
    IProcessor(IProcessor&&) = default;
    IProcessor& operator=(IProcessor&&) = default;

    virtual void Init(float sample_rate) = 0;
    virtual void Process(Partials& partials) = 0;
    virtual void OnUpdateTick(const SynthParam& params, int skip, int module_idx) = 0;
    virtual void OnNoteOn(int note) = 0;
    virtual void OnNoteOff() = 0;
};
}