#pragma once

#include <string_view>

namespace mana {
class ModuContainer {
public:
    ModuContainer() = default;
    virtual ~ModuContainer() = default;

    virtual void BeginHighlightModulator(std::string_view id) = 0;
    virtual void StopHighliteModulator() = 0;
};
}