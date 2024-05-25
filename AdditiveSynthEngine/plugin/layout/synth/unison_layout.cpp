#include "unison_layout.h"

#include <ranges>
#include "layout/gui_param_pack.h"
#include "engine/synth_params.h"

namespace mana {
UnisonLayout::UnisonLayout(SynthParams& params) {
    auto& bank = params.GetParamBank();
    type_.SetParameter(bank.GetParamPtr<IntChoiceParameter>("unison.type"));
    num_voice_.SetParameter(bank.GetParamPtr<IntParameter>("unison.num_voice"));
    pitch_.set_parameter(bank.GetParamPtr("unison.pitch"));
    phase_.set_parameter(bank.GetParamPtr("unison.phase"));
    pan_.set_parameter(bank.GetParamPtr("unison.pan"));
}

void UnisonLayout::Paint() {
    pitch_.display();
    phase_.display();
    pan_.display();
    type_.Paint();
    num_voice_.Paint();
}

void UnisonLayout::SetBounds(Rectangle bound) {
    auto left_x = bound.x + bound.width - 50.0f;
    type_.SetBounds({ bound.x + bound.width - 50.0f - 24.0f, bound.y,50 ,16 });
    num_voice_.SetBounds({ bound.x + bound.width - 24.0f, bound.y,24, 16 });
    pitch_.set_bound({ left_x, bound.y + 16,50,50 });
    phase_.set_bound({ left_x, bound.y + 16 + 50,50,50 });
    pan_.set_bound({ left_x, bound.y + 16 + 100,50,50 });
}
}