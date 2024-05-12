#pragma once

#include <vector>
#include "Modulator.h"

namespace mana {
class ModulatorBank {
public:
    ModulatorBank();
    Modulator* GetModulatorPtr(std::string_view id);
    std::vector<std::string_view> GetModulatorsIds() const;

    void Init(float sample_rate, float update_rate);
    void PrepareParams(OscillorParams& params);
    void OnUpdateTick();
    void OnNoteOn(int note);
    void OnNoteOff();
private:
    std::vector<std::shared_ptr<Modulator>> modulators_;
};
}