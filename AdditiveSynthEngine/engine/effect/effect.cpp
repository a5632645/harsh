#include "effect.h"

#include "octaver.h"
#include "reverb.h"
#include "chrous.h"
#include "phaser.h"
#include "scramble.h"
#include "Blur.h"
#include "decay.h"

namespace mana {
void Effect::Init(float sample_rate) {
    processers_[param::EffectType::ParamEnum::kOctaver] = std::make_unique<Octaver>();
    processers_[param::EffectType::ParamEnum::kReverb] = std::make_unique<Reverb>();
    processers_[param::EffectType::ParamEnum::kChorus] = std::make_unique<Chorus>();
    processers_[param::EffectType::ParamEnum::kPhaser] = std::make_unique<Phaser>();
    processers_[param::EffectType::ParamEnum::kScramble] = std::make_unique<Scramble>();
    processers_[param::EffectType::ParamEnum::kBlur] = std::make_unique<Blur>();
    processers_[param::EffectType::ParamEnum::kDecay] = std::make_unique<Decay>();

    for (auto&&[k, v] : processers_) {
        v->Init(sample_rate);
    }
    p_processor_ = processers_.begin()->second.get();
}

void Effect::Process(Partials& partials) {
    if (!is_enable_) return;
    p_processor_->Process(partials);
}

void Effect::OnUpdateTick(const SynthParam& params, int skip, int /*module_idx*/) {
    is_enable_ = params.effects[effect_idx_].is_enable;
    effect_type_ = param::EffectType::GetEnum(
        params.effects[effect_idx_].effect_type);
    p_processor_ = processers_.at(effect_type_).get();

    p_processor_->OnUpdateTick(params, skip, effect_idx_);
}

void Effect::OnNoteOn(int note) {
    for (auto&&[k, v] : processers_) {
        v->OnNoteOn(note);
    }
}

void Effect::OnNoteOff() {
    for (auto&&[k, v] : processers_) {
        v->OnNoteOff();
    }
}
}