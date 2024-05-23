#pragma once

#include <raylib.h>

namespace mana {
class LayoutBase {
public:
    LayoutBase() = default;
    virtual ~LayoutBase() = default;
    LayoutBase(const LayoutBase&) = default;
    LayoutBase& operator=(const LayoutBase&) = default;
    LayoutBase(LayoutBase&&) noexcept = default;
    LayoutBase& operator=(LayoutBase&) = default;

    virtual void Paint() = 0;
    virtual void SetBounds(Rectangle bound) = 0;

    std::string_view GetId() const { return id_; }
protected:
    std::string id_;
};
}