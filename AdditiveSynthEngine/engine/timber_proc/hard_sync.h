#pragma once

#include <array>
#include "engine/IProcessor.h"
#include "param/timber_proc_param.h"
#include "engine/EngineConfig.h"

namespace mana {
class HardSync : public IProcessor {
public:
    HardSync(const param::HardSyncParam& p);

    // 通过 IProcessor 继承
    void Init(float sample_rate) override;
    void Process(Partials & partials) override;
    void OnUpdateTick(int skip) override;
    void OnNoteOn(int note) override;
    void OnNoteOff() override;
private:
    const param::HardSyncParam& param_;
    std::array<float, kNumPartials> saved_gains_{};
};
}