#pragma once

#include "engine/IProcessor.h"
#include "param/timber.h"
#include "sync.h"
#include "dual_saw.h"

namespace mana {
class Timber : public IProcessor {
public:
    // 通过 IProcessor 继承
    void Init(float sample_rate) override;
    void Process(Partials & partials) override;
    void OnUpdateTick(const SynthParam& params, int skip) override;
    void OnNoteOn(int note) override;
    void OnNoteOff() override;
private:
    param::TimberType::TimberEnum timber_type_;

    // =====================
    // sub processor
    // =====================
    DualSaw dual_saw_;
    Sync sync_;
};
}