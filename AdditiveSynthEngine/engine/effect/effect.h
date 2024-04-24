#pragma once

#include <unordered_map>
#include "engine/IProcessor.h"
#include "param/effect_param.h"

namespace mana {
class Effect : public IProcessor {
public:
    Effect(int idx) : effect_idx_(idx) {}
    // 通过 IProcessor 继承
    void Init(float sample_rate) override;
    void Process(Partials & partials) override;
    void OnUpdateTick(const SynthParam& params, int skip, int module_idx) override;
    void OnNoteOn(int note) override;
    void OnNoteOff() override;
private:
    const int effect_idx_;

    bool is_enable_{};
    param::EffectType::EffectTypeEnum effect_type_{};
    std::unordered_map<param::EffectType::EffectTypeEnum, std::unique_ptr<IProcessor>> processers_;
    IProcessor* p_processor_{};
};
}