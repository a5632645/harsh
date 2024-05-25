#include "filter_route_layout.h"

#include "engine/synth.h"

namespace mana {
FilterRouteLayout::FilterRouteLayout(Synth& synth) {
    auto& bank = synth.GetParamBank();

    filter1_enable_.SetParameter(bank.GetParamPtr<BoolParameter>("filter0.enable"));
    filter2_enable_.SetParameter(bank.GetParamPtr<BoolParameter>("filter1.enable"));
    stream_type_.SetParameter(bank.GetParamPtr<BoolParameter>("filter.stream_type"));
}

void FilterRouteLayout::Paint() {
    filter1_enable_.Paint();
    filter2_enable_.Paint();
    stream_type_.Paint();
}

void FilterRouteLayout::SetBounds(Rectangle bound) {
    filter1_enable_.SetBounds({ bound.x,bound.y,16,16 });
    filter2_enable_.SetBounds({ bound.x,bound.y + 16,16,16 });
    stream_type_.SetBounds({ bound.x,bound.y + 32,16,16 });
}
}