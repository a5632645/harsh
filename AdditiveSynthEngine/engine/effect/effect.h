#pragma once

#include <unordered_map>
#include "engine/IProcessor.h"
#include "param/effect_param.h"

namespace mana {
class Effect : public IProcessor {
public:
    Effect(int idx);
    void Init(float sample_rate, float update_rate) override;
    void Process(Partials & partials) override;
    void OnUpdateTick(const OscillorParams& params, int skip, int module_idx) override;
    void OnNoteOn(int note) override;
    void OnNoteOff() override;
private:
    const int effect_idx_;

    bool is_enable_{};
    param::EffectType::ParamEnum effect_type_{};
    std::unordered_map<param::EffectType::ParamEnum, std::unique_ptr<IProcessor>> processers_;
    IProcessor* p_processor_{};
};
}