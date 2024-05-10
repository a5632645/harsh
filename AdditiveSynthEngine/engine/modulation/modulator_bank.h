#pragma once

#include <vector>
#include "Modulator.h"

namespace mana {
class ModulatorBank : public IProcessor {
public:
    ModulatorBank();
    Modulator* GetModulatorPtr(std::string_view id);
    std::vector<std::string_view> GetModulatorsIds() const;

    void Init(float sample_rate, float update_rate) override;
    void Process(Partials & partials) override;
    void OnUpdateTick(const OscillorParams & params, int skip, int module_idx) override;
    void OnNoteOn(int note) override;
    void OnNoteOff() override;
private:
    std::vector<std::shared_ptr<Modulator>> modulators_;
};
}