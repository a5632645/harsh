#include "unison_layout.h"

#include <ranges>
#include "param/timber_param.h"
#include "layout/gui_param_pack.h"

namespace mana {
UnisonLayout::UnisonLayout(SynthParams& params) {
    auto& bank = params.GetParamBank();
    type_.SetParameter(bank.GetParamPtr<IntParameter>("unison.type"));
    num_voice_.SetParameter(bank.GetParamPtr<IntParameter>("unison.num_voice"));
    pitch_.set_parameter(bank.GetParamPtr("unison.pitch"));
    SetSingeKnobInfo(pitch_, param::Unison_Pitch{});
    phase_.set_parameter(bank.GetParamPtr("unison.phase"));
    pan_.set_parameter(bank.GetParamPtr("unison.pan"));

    num_voice_.SetChoices(std::ranges::views::iota(1, 9 + 1) | std::ranges::views::transform(
        [](int v) {return std::to_string(v); }));
}

void UnisonLayout::Paint() {
    pitch_.display();
    phase_.display();
    pan_.display();
    type_.Paint();
    num_voice_.Paint();
}

void UnisonLayout::SetBounds(Rectangle bound) {
    type_.SetBounds({ bound.x, bound.y,bound.width / 2 ,16 });
    num_voice_.SetBounds({ bound.x + bound.width / 2, bound.y,bound.width / 2, 16 });
    pitch_.set_bound({ bound.x, bound.y + 16,50,50 });
    phase_.set_bound({ bound.x + 50, bound.y + 16,50,50 });
    pitch_.set_bound({ bound.x + 100, bound.y + 16,50,50 });
}
}