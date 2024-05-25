#pragma once

#include "engine/forward_decalre.h"
#include "filter_layout.h"
#include "filter_route_layout.h"

namespace mana {
class FinalFilterLayout {
public:
    FinalFilterLayout(Synth& synth);
    void Paint();
    void SetBounds(Rectangle bound);
private:
    FilterRouteLayout route_;
    FilterLayout filter1_;
    FilterLayout filter2_;
};
}