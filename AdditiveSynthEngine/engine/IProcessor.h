#pragma once

#include "partials.h"

namespace mana {
class IProcessor {
public:
    virtual ~IProcessor() = default;

    virtual void Init(float sample_rate) = 0;
    virtual void Process(Partials& partials) = 0;
    virtual void OnUpdateTick(int skip) = 0;
    virtual void OnNoteOn(int note) = 0;
    virtual void OnNoteOff() = 0;
};
}