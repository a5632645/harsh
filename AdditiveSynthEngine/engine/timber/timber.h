#pragma once

#include "engine/IProcessor.h"
#include "param/timber_param.h"
#include "sync.h"
#include "dual_saw.h"

namespace mana {
class Timber : public IProcessor {
public:
    void Init(float sample_rate) override;
    void Process(Partials & partials) override;
    void OnUpdateTick(const SynthParam& params, int skip, int module_idx) override;
    void OnNoteOn(int note) override;
    void OnNoteOff() override;
private:
    param::TimberType::ParamEnum timber_type_;

    bool is_note_on_{};
    // =====================
    // sub processor
    // =====================
    DualSaw dual_saw_;
    Sync sync_;
};
}