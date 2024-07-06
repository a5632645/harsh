#pragma once

#include <string_view>

namespace mana {
class TimeFxBase {
public:
    virtual ~TimeFxBase() = default;
    virtual void Process(float* pbuffer, int num) = 0;
    virtual std::string_view GetFxType() const = 0;
};
}