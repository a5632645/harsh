#include "effect.h"

#include "octaver.h"
#include "reverb.h"
#include "chrous.h"
#include "phaser.h"
#include "scramble.h"
#include "decay.h"
#include "harmonic_delay.h"
#include "spectral_gate.h"

namespace mana {
Effect::Effect(int idx) : effect_idx_(idx) {
    processers_[param::EffectType::ParamEnum::kOctaver] = std::make_unique<Octaver>();
    processers_[param::EffectType::ParamEnum::kReverb] = std::make_unique<Reverb>();
    processers_[param::EffectType::ParamEnum::kChorus] = std::make_unique<Chorus>();
    processers_[param::EffectType::ParamEnum::kPhaser] = std::make_unique<Phaser>();
    processers_[param::EffectType::ParamEnum::kScramble] = std::make_unique<Scramble>();
    processers_[param::EffectType::ParamEnum::kDecay] = std::make_unique<Decay>();
    processers_[param::EffectType::ParamEnum::kHarmonicDelay] = std::make_unique<HarmonicDelay>();
    processers_[param::EffectType::ParamEnum::kSpectralGate] = std::make_unique<SpectralGate>();

    p_processor_ = processers_.begin()->second.get();
}

void Effect::Init(float sample_rate, float update_rate) {
    for (auto&&[k, v] : processers_) {
        v->Init(sample_rate, update_rate);
    }
}

void Effect::PrepareParams(OscillorParams& params) {
    is_enable_ = params.GetParam<BoolParameter>("effect{}.enable", effect_idx_);
    effect_type_arg_ = params.GetParam<IntParameter>("effect{}.type", effect_idx_);

    for (int i = 0; auto & parg : effect_args_.args) {
        parg = params.GetPolyFloatParam("effect{}.arg{}", effect_idx_, i++);
    }
    curve_manager_ = &params.GetParentSynthParams().GetCurveManager();

    for (auto& e : processers_) {
        e.second->PrepareParams(params);
    }
}

void Effect::Process(Partials& partials) {
    if (!is_enable_->GetBool()) return;
    p_processor_->Process(partials);
}

void Effect::OnUpdateTick() {
    effect_type_ = param::EffectType::GetEnum(effect_type_arg_->GetInt());

    p_processor_ = processers_.at(effect_type_).get();
    p_processor_->OnUpdateTick(effect_args_, *curve_manager_);
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