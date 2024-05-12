#pragma once

#include <unordered_map>
#include "param/effect_param.h"
#include "effect_base.h"
#include <engine/oscillor_param.h>

namespace mana {
class Effect {
public:
    Effect(int idx);
    void Init(float sample_rate, float update_rate);
    void PrepareParams(OscillorParams& params);
    void Process(Partials & partials);
    void OnUpdateTick();
    void OnNoteOn(int note);
    void OnNoteOff();
private:
    const int effect_idx_;
    BoolParameter* is_enable_{};
    IntParameter* effect_type_arg_{};
    EffectParams effect_args_;
    CurveManager* curve_manager_;
    param::EffectType::ParamEnum effect_type_{};
    std::unordered_map<param::EffectType::ParamEnum, std::unique_ptr<EffectBase>> processers_;
    EffectBase* p_processor_{};
};
}