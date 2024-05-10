#pragma once

#include "engine/IProcessor.h"

#include <random>
#include "param/standard_param.h"

namespace mana {
class PhaseProcessor : public IProcessor {
public:
    void Init(float sample_rate, float update_rate) override;
    void OnUpdateTick(const OscillorParams& params, int skip, int module_idx) override;
    void OnNoteOn(int note) override;
    void OnNoteOff() override;
    void Process(Partials& partials) override;
private:
    inline void DoResetOnce(Partials& partials);
    inline void DoReset(Partials& partials);
    inline void DoDispersion(Partials& partials);
    inline void DoSpectralRandom(Partials& partials);
    inline void DoPowDistribute(Partials& partials);

    bool note_on_once_flag_{};
    std::random_device rand_;

    param::PhaseType::ParamEnum process_type_{};
    float process_arg0_{};
    float process_arg1_{};
};
}