#pragma once

#include "filter.h"
#include "engine/modulation/Parameter.h"

namespace mana {
class FilterWrap {
public:
    FilterWrap();

    void Init(float sample_rate, float update_rate);
    void PrepareParams(OscillorParams& params);
    void Process(Partials& partials);
    void OnUpdateTick();
    void OnNoteOn(int note);
    void OnNoteOff();
private:
    std::vector<float> filter_out_;
    Filter filter1_;
    Filter filter2_;

    BoolParameter* stream_type_{};
    BoolParameter* filter1_enable_{};
    BoolParameter* filter2_enable_{};
};
}