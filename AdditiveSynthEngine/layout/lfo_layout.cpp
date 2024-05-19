#include "lfo_layout.h"

#include <format>
#include "layout/gui_param_pack.h"
#include "param/lfo_param.h"
#include "engine/synth.h"

namespace mana {
LfoLayout::LfoLayout(Synth& synth, int idx) {
    const auto& bank = synth.GetParamBank();

    rate_.set_parameter(bank.GetParamPtr(std::format("lfo{}.rate", idx)));
    //SetSingeKnobInfo(rate_, param::LFO_Rate{});
    start_phase_.set_parameter(bank.GetParamPtr(std::format("lfo{}.start_phase", idx)));
    //SetSingeKnobInfo(start_phase_, param::LFO_Phase{});
    level_.set_parameter(bank.GetParamPtr(std::format("lfo{}.level", idx)));
    //SetSingeKnobInfo(level_, param::LFO_Level{});

    restart_.SetParameter(bank.GetParamPtr<BoolParameter>(std::format("lfo{}.restart", idx)));
    restart_.SetText("restart");
    wave_type_.SetParameter(bank.GetParamPtr<IntChoiceParameter>(std::format("lfo{}.wave_type", idx)));
    //wave_type_.SetChoices(param::LFO_WaveType::kNames);
}

void LfoLayout::Paint() {
    rate_.display();
    start_phase_.display();
    level_.display();
    restart_.Paint();
    wave_type_.Paint();
}

void LfoLayout::SetBounds(Rectangle bound) {
    rate_.set_bound(bound.x, bound.y, 50, 50);
    start_phase_.set_bound({ bound.x + 50,bound.y,50,50 });
    level_.set_bound({ bound.x + 100,bound.y,50,50 });
    restart_.SetBounds({ bound.x + 150,bound.y, 16,16 });
    wave_type_.SetBounds({ bound.x + 216,bound.y,100,16 });
}
}