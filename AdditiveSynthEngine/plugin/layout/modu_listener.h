#pragma once

#include <string_view>

namespace mana {
class ModulationActionListener {
public:
    virtual ~ModulationActionListener() = default;

    virtual void BeginHighlightModulator(std::string_view modulator_id) = 0;
    virtual void StopHighliteModulator() = 0;
};
}