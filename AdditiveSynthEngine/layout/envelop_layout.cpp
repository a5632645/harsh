#include "envelop_layout.h"

#include "engine/synth.h"
#include "param/envelop_param.h"
#include "layout/gui_param_pack.h"

namespace mana {
EnvelopLayout::EnvelopLayout(Synth& synth, int idx) {
    const auto& param_bank = synth.GetParamBank();

    attack_.set_parameter(param_bank.GetParamPtr("envelop{}.attack", idx));
    SetSingeKnobInfo(attack_, param::Env_Attack{});
    decay_.set_parameter(param_bank.GetParamPtr("envelop{}.decay", idx));
    SetSingeKnobInfo(decay_, param::Env_Decay{});
    sustain_.set_parameter(param_bank.GetParamPtr("envelop{}.sustain", idx));
    SetSingeKnobInfo(sustain_, param::Env_Sustain{});
    release_.set_parameter(param_bank.GetParamPtr("envelop{}.release", idx));
    SetSingeKnobInfo(release_, param::Env_Release{});
}

void EnvelopLayout::Paint() {
    attack_.display();
    decay_.display();
    sustain_.display();
    release_.display();
}

void EnvelopLayout::SetBounds(Rectangle bound) {
    attack_.set_bound({ bound.x, bound.y, 50, 50 });
    decay_.set_bound({ bound.x + 50, bound.y, 50, 50 });
    sustain_.set_bound({ bound.x + 100, bound.y, 50, 50 });
    release_.set_bound({ bound.x + 150, bound.y, 50, 50 });
}
}