#include "synth_params.h"

#include "param/standard_param.h"
#include "param/timber_param.h"
#include "param/dissonance_param.h"
#include "param/effect_param.h"
#include "param/envelop_param.h"
#include "param/lfo_param.h"
#include "param/filter_param.h"
#include "param/resynthsis_param.h"
#include "param/unison_param.h"

namespace mana {
static constexpr ParamRange kUnitRange{ 0.0f,1.0f };
}

namespace mana {
SynthParams::SynthParams(std::shared_ptr<ParamCreator> creator) {
    using enum ModulationType;

    //param_bank_.AddParameter(param::PitchBend::CreateParam(kPoly, "pitch_bend"));
    param_bank_.AddParameter(creator->CreateFloatParameter({
        .type = kPoly,
        .id = "pitch_bend",
        .name = "pitch_bend",
        .vmin = param::PitchBend::kMin,
        .vmax = param::PitchBend::kMax,
        .vdefault = param::PitchBend::kDefault }));

    //param_bank_.AddParameter(param::OutputGain::CreateParam(kDisable, "output_gain"));
    param_bank_.AddParameter(creator->CreateFloatParameter({
        .type = kDisable,
        .id = "output_gain",
        .name = "output_gain",
        .vmin = param::OutputGain::kMin,
        .vmax = param::OutputGain::kMax,
        .vdefault = param::OutputGain::kDefault }));

    //param_bank_.AddParameter(param::PhaseType::CreateParam(kDisable, "phase.type"));
    param_bank_.AddParameter(creator->CreateIntChoiceParameter({
        .id = "phase.type",
        .name = "phase.type",
        .choices = {param::PhaseType::kNames.begin(), param::PhaseType::kNames.end()},
        .vdefault = 0 }));
    for (int arg_idx = 0; arg_idx < 2; ++arg_idx) {
        //param_bank_.AddOrCreateIfNull(kPoly, kUnitRange, std::format("arg{}", arg_idx), "phase.arg{}", arg_idx);
        param_bank_.AddParameter(creator->CreateFloatParameter({
            .type = kPoly,
            .id = std::format("phase.arg{}", arg_idx),
            .name = std::format("phase.arg{}", arg_idx),
            .vmin = 0.0f,
            .vmax = 1.0f,
            .vdefault = 0.0f }));
    }

    for (int osc_idx = 0; osc_idx < 2; ++osc_idx) {
        //param_bank_.AddParameter(param::TimberType::CreateParam(kDisable, "timber.osc{}.type", osc_idx));
        param_bank_.AddParameter(creator->CreateIntChoiceParameter({
            .id = std::format("timber.osc{}.type", osc_idx),
            .name = std::format("timber.osc{}.type", osc_idx),
            .choices = {param::TimberType::kNames.begin(), param::TimberType::kNames.end()},
            .vdefault = 0 }));
        for (int arg_idx = 0; arg_idx < 4; ++arg_idx) {
            //param_bank_.AddParameter(CreateAlterParam(kPoly, "arg{1}", "timber.osc{}.arg{}", osc_idx, arg_idx));
            param_bank_.AddParameter(creator->CreateFloatParameter({
                .type = kPoly,
                .id = std::format("timber.osc{}.arg{}", osc_idx, arg_idx),
                .name = std::format("osc{}.arg{}", osc_idx, arg_idx),
                .vmin = 0.0f,
                .vmax = 1.0f,
                .vdefault = 0.0f }));
        }
    }
    //param_bank_.AddParameter(param::Timber_Osc2Shift::CreateParam(kPoly, "timber.osc2_shift"));
    param_bank_.AddParameter(creator->CreateFloatParameter({
        .type = kPoly,
        .id = "timber.osc2_shift",
        .name = "osc2_shift",
        .vmin = param::Timber_Osc2Shift::kMin,
        .vmax = param::Timber_Osc2Shift::kMax,
        .vdefault = param::Timber_Osc2Shift::kDefault }));
    //param_bank_.AddParameter(param::Timber_Osc2Beating::CreateParam(kPoly, "timber.osc2_beating"));
    param_bank_.AddParameter(creator->CreateFloatParameter({
        .type = kPoly,
        .id = "timber.osc2_beating",
        .name = "osc2_beating",
        .vmin = param::Timber_Osc2Beating::kMin,
        .vmax = param::Timber_Osc2Beating::kMax,
        .vdefault = param::Timber_Osc2Beating::kDefault }));
    //param_bank_.AddParameter(param::Timber_OscGain::CreateParam(kPoly, "timber.osc1_gain"));
    param_bank_.AddParameter(creator->CreateFloatParameter({
        .type = kPoly,
        .id = "timber.osc1_gain",
        .name = "osc1_gain",
        .vmin = param::Timber_OscGain::kMin,
        .vmax = param::Timber_OscGain::kMax,
        .vdefault = param::Timber_OscGain::kDefault }));
    //param_bank_.AddParameter(param::Timber_OscGain::CreateParam(kPoly, "timber.osc2_gain"));
    param_bank_.AddParameter(creator->CreateFloatParameter({
        .type = kPoly,
        .id = "timber.osc2_gain",
        .name = "osc2_gain",
        .vmin = param::Timber_OscGain::kMin,
        .vmax = param::Timber_OscGain::kMax,
        .vdefault = param::Timber_OscGain::kDefault }));
    //param_bank_.AddParameter(param::Timber_OscGain::CreateParam(kPoly, "timber.osc2_gain"));

    //param_bank_.AddParameter(param::Unison_Type::CreateParam(kDisable, "unison.type"));
    param_bank_.AddParameter(creator->CreateIntChoiceParameter({
        .id = "unison.type",
        .name = "unison.type",
        .choices = {param::Unison_Type::kNames.begin(), param::Unison_Type::kNames.end()},
        .vdefault = 0 }));
    //param_bank_.AddParameter(param::Unison_NumVoice::CreateParam(kDisable, "unison.num_voice"));
    param_bank_.AddParameter(creator->CreateIntParameter({
        .id = "unison.num_voice",
        .name = "unison.num_voice",
        .vmin = param::Unison_NumVoice::kMin,
        .vmax = param::Unison_NumVoice::kMax,
        .vdefault = param::Unison_NumVoice::kDefault }));
    //param_bank_.AddParameter(param::Unison_Pitch::CreateParam(kPoly, "unison.pitch"));
    param_bank_.AddParameter(creator->CreateFloatParameter({
        .type = kPoly,
        .id = "unison.pitch",
        .name = "unison.pitch",
        .vmin = param::Unison_Pitch::kMin,
        .vmax = param::Unison_Pitch::kMax,
        .vdefault = param::Unison_Pitch::kDefault }));
    //param_bank_.AddParameter(param::Unison_Phase::CreateParam(kPoly, "unison.phase"));
    param_bank_.AddParameter(creator->CreateFloatParameter({
        .type = kPoly,
        .id = "unison.phase",
        .name = "unison.phase",
        .vmin = param::Unison_Phase::kMin,
        .vmax = param::Unison_Phase::kMax,
        .vdefault = param::Unison_Phase::kDefault }));
    //param_bank_.AddParameter(param::Unison_Pan::CreateParam(kPoly, "unison.pan"));
    param_bank_.AddParameter(creator->CreateFloatParameter({
        .type = kPoly,
        .id = "unison.pan",
        .name = "unison.pan",
        .vmin = param::Unison_Pan::kMin,
        .vmax = param::Unison_Pan::kMax,
        .vdefault = param::Unison_Pan::kDefault }));

    //param_bank_.AddOrCreateIfNull<BoolParameter>(kDisable, kUnitRange, "enable", "dissonance.enable");
    param_bank_.AddParameter(creator->CreateBoolParameter({
        .id = "dissonance.enable",
        .name = "dissonance.enable",
        .vdefault = false }));
    //param_bank_.AddParameter(param::DissonanceType::CreateParam(kDisable, "dissonance.type"));
    param_bank_.AddParameter(creator->CreateIntChoiceParameter({
        .id = "dissonance.type",
        .name = "dissonance.type",
        .choices = {param::DissonanceType::kNames.begin(), param::DissonanceType::kNames.end()},
        .vdefault = 0 }));
    for (int arg_idx = 0; arg_idx < 2; ++arg_idx) {
        //param_bank_.AddParameter(CreateAlterParam(kPoly, "arg{}", "dissonance.arg{}", arg_idx));
        param_bank_.AddParameter(creator->CreateFloatParameter({
            .type = kPoly,
            .id = std::format("dissonance.arg{}", arg_idx),
            .name = std::format("dissonance.arg{}", arg_idx),
            .vmin = 0.0f,
            .vmax = 1.0f,
            .vdefault = 0.0f }));
    }

    //param_bank_.AddOrCreateIfNull<BoolParameter>(kDisable, kUnitRange, "filter_stream", "filter.stream_type");
    param_bank_.AddParameter(creator->CreateBoolParameter({
        .id = "filter.stream_type",
        .name = "filter.stream_type",
        .vdefault = false }));
    for (int filter_idx = 0; filter_idx < 2; ++filter_idx) {
        //param_bank_.AddOrCreateIfNull<BoolParameter>(kDisable, kUnitRange, std::format("filter{} enable", filter_idx), "filter{}.enable", filter_idx);
        param_bank_.AddParameter(creator->CreateBoolParameter({
            .id = std::format("filter{}.enable", filter_idx),
            .name = std::format("filter{}.enable", filter_idx),
            .vdefault = false }));
        //param_bank_.AddParameter(param::Filter_Type::CreateParam(kDisable, "filter{}.type", filter_idx));
        param_bank_.AddParameter(creator->CreateIntChoiceParameter({
            .id = std::format("filter{}.type", filter_idx),
            .name = std::format("filter{}.type", filter_idx),
            .choices = {param::Filter_Type::kNames.begin(), param::Filter_Type::kNames.end()},
            .vdefault = 0 }));
        for (int arg_idx = 0; arg_idx < 8; ++arg_idx) {
            //param_bank_.AddParameter(CreateAlterParam(kPoly, "arg{1}", "filter{}.arg{}", filter_idx, arg_idx));
            param_bank_.AddParameter(creator->CreateFloatParameter({
                .type = kPoly,
                .id = std::format("filter{}.arg{}", filter_idx, arg_idx),
                .name = std::format("filter{}.arg{}", filter_idx, arg_idx),
                .vmin = 0.0f,
                .vmax = 1.0f,
                .vdefault = 0.0f }));
        }
    }

    // resynthsis
    //param_bank_.AddOrCreateIfNull<BoolParameter>(kDisable, kUnitRange, "", "resynthsis.enable");
    param_bank_.AddParameter(creator->CreateBoolParameter({
        .id = "resynthsis.enable",
        .name = "resynthsis.enable",
        .vdefault = false }));
    //param_bank_.AddOrCreateIfNull<BoolParameter>(kDisable, kUnitRange, "", "resynthsis.formant_remap");
    param_bank_.AddParameter(creator->CreateBoolParameter({
        .id = "resynthsis.formant_remap",
        .name = "resynthsis.formant_remap",
        .vdefault = false }));
    for (int arg_idx = 0; arg_idx < 7; ++arg_idx) {
        //param_bank_.AddOrCreateIfNull(kPoly, kUnitRange, "", "resynthsis.arg{}", arg_idx);
        param_bank_.AddParameter(creator->CreateFloatParameter({
            .type = kPoly,
            .id = std::format("resynthsis.arg{}", arg_idx),
            .name = std::format("resynthsis.arg{}", arg_idx),
            .vmin = 0.0f,
            .vmax = 1.0f,
            .vdefault = 0.0f }));
    }

    // effect
    for (int effect_idx = 0; effect_idx < 5; ++effect_idx) {
        //param_bank_.AddOrCreateIfNull<BoolParameter>(kDisable, kUnitRange, "", "effect{}.enable", effect_idx);
        param_bank_.AddParameter(creator->CreateBoolParameter({
            .id = std::format("effect{}.enable", effect_idx),
            .name = std::format("effect{}.enable", effect_idx),
            .vdefault = false }));
        //param_bank_.AddParameter(param::EffectType::CreateParam(kDisable, "effect{}.type", effect_idx));
        param_bank_.AddParameter(creator->CreateIntChoiceParameter({
            .id = std::format("effect{}.type", effect_idx),
            .name = std::format("effect{}.type", effect_idx),
            .choices = {param::EffectType::kNames.begin(), param::EffectType::kNames.end()},
            .vdefault = 0 }));
        for (int arg_idx = 0; arg_idx < 6; ++arg_idx) {
            //param_bank_.AddOrCreateIfNull(kPoly, kUnitRange, std::format("arg{}", arg_idx), "effect{}.arg{}", effect_idx, arg_idx);
            param_bank_.AddParameter(creator->CreateFloatParameter({
                .type = kPoly,
                .id = std::format("effect{}.arg{}", effect_idx, arg_idx),
                .name = std::format("effect{}.arg{}", effect_idx, arg_idx),
                .vmin = 0.0f,
                .vmax = 1.0f,
                .vdefault = 0.0f }));
        }
    }

    // lfo
    for (int lfo_idx = 0; lfo_idx < 8; ++lfo_idx) {
        //param_bank_.AddOrCreateIfNull<BoolParameter>(kDisable, kUnitRange, "", "lfo{}.restart", lfo_idx);
        param_bank_.AddParameter(creator->CreateBoolParameter({
            .id = std::format("lfo{}.restart", lfo_idx),
            .name = std::format("lfo{}.restart", lfo_idx),
            .vdefault = false }));
        //param_bank_.AddParameter(param::LFO_Rate::CreateParam(kPoly, "lfo{}.rate", lfo_idx));
        param_bank_.AddParameter(creator->CreateFloatParameter({
            .type = kPoly,
            .id = std::format("lfo{}.rate", lfo_idx),
            .name = std::format("lfo{}.rate", lfo_idx),
            .vmin = param::LFO_Rate::kMin,
            .vmax = param::LFO_Rate::kMax,
            .vdefault = param::LFO_Rate::kDefault,
            .vblend = 0.3f }));
        //param_bank_.AddParameter(param::LFO_Phase::CreateParam(kPoly, "lfo{}.start_phase", lfo_idx));
        param_bank_.AddParameter(creator->CreateFloatParameter({
            .type = kPoly,
            .id = std::format("lfo{}.start_phase", lfo_idx),
            .name = std::format("lfo{}.start_phase", lfo_idx),
            .vmin = param::LFO_Phase::kMin,
            .vmax = param::LFO_Phase::kMax,
            .vdefault = param::LFO_Phase::kDefault }));
        //param_bank_.AddParameter(param::LFO_Level::CreateParam(kPoly, "lfo{}.level", lfo_idx));
        param_bank_.AddParameter(creator->CreateFloatParameter({
            .type = kPoly,
            .id = std::format("lfo{}.level", lfo_idx),
            .name = std::format("lfo{}.level", lfo_idx),
            .vmin = param::LFO_Level::kMin,
            .vmax = param::LFO_Level::kMax,
            .vdefault = param::LFO_Level::kDefault }));
        //param_bank_.AddParameter(param::LFO_WaveType::CreateParam(kPoly, "lfo{}.wave_type", lfo_idx));
        param_bank_.AddParameter(creator->CreateIntChoiceParameter({
            .id = std::format("lfo{}.wave_type", lfo_idx),
            .name = std::format("lfo{}.wave_type", lfo_idx),
            .choices = {param::LFO_WaveType::kNames.begin(), param::LFO_WaveType::kNames.end()},
            .vdefault = 0 }));
    }
    // envelop
    for (int env_idx = 0; env_idx < 8; ++env_idx) {
        //param_bank_.AddParameter(param::Env_Attack::CreateParam(kPoly, "envelop{}.attack", env_idx));
        param_bank_.AddParameter(creator->CreateFloatParameter({
            .type = kPoly,
            .id = std::format("envelop{}.attack", env_idx),
            .name = std::format("envelop{}.attack", env_idx),
            .vmin = param::Env_Attack::kMin,
            .vmax = param::Env_Attack::kMax,
            .vdefault = param::Env_Attack::kDefault }));
        //param_bank_.AddParameter(param::Env_Decay::CreateParam(kPoly, "envelop{}.decay", env_idx));
        param_bank_.AddParameter(creator->CreateFloatParameter({
            .type = kPoly,
            .id = std::format("envelop{}.decay", env_idx),
            .name = std::format("envelop{}.decay", env_idx),
            .vmin = param::Env_Decay::kMin,
            .vmax = param::Env_Decay::kMax,
            .vdefault = param::Env_Decay::kDefault }));
        //param_bank_.AddParameter(param::Env_Sustain::CreateParam(kPoly, "envelop{}.sustain", env_idx));
        param_bank_.AddParameter(creator->CreateFloatParameter({
            .type = kPoly,
            .id = std::format("envelop{}.sustain", env_idx),
            .name = std::format("envelop{}.sustain", env_idx),
            .vmin = param::Env_Sustain::kMin,
            .vmax = param::Env_Sustain::kMax,
            .vdefault = param::Env_Sustain::kDefault }));
        //param_bank_.AddParameter(param::Env_Release::CreateParam(kPoly, "envelop{}.release", env_idx));
        param_bank_.AddParameter(creator->CreateFloatParameter({
            .type = kPoly,
            .id = std::format("envelop{}.release", env_idx),
            .name = std::format("envelop{}.release", env_idx),
            .vmin = param::Env_Release::kMin,
            .vmax = param::Env_Release::kMax,
            .vdefault = param::Env_Release::kDefault }));
    }
    // custom curves
    curve_bank_.AddCurve("resynthsis.formant_remap")
        .AddCurve("resynthsis.pos_offset")
        .AddCurve("effect.harmonic_delay.time")
        .AddCurve("effect.harmonic_delay.feedback")
        .AddQuantizeMap({ 0.f,1.f,2.f,3.f,4.f,5.f,6.f,7.f,8.f,9.f,10.f,11.f },
                        "dissonance.pitch_quantize");
    for (int i = 0; i < 8; ++i) {
        curve_bank_.AddCurve("lfo{}.wave", i);
    }
}

std::shared_ptr<ModulationConfig> SynthParams::FindModulation(std::string_view modulator_id, std::string_view param_id) {
    auto existed_it = std::ranges::find_if(modulation_configs_, [modulator_id, param_id](std::shared_ptr<ModulationConfig>& cfg) {
        return cfg->modulator_id == modulator_id && cfg->param_id == param_id;
    });

    if (existed_it != modulation_configs_.cend()) {
        return *existed_it;
    }
    else {
        return {};
    }
}

void SynthParams::AddModulation(std::shared_ptr<ModulationConfig> config) {
    modulation_configs_.emplace_back(config);

    for (auto* l : modulation_listeners_) {
        l->OnModulationAdded(config);
    }
}

void SynthParams::RemoveModulation(ModulationConfig& config) {
    auto modulator_id = config.modulator_id;
    auto param_id = config.param_id;

    auto it = std::ranges::find_if(modulation_configs_, [config](std::shared_ptr<ModulationConfig>& cfg) {
        return cfg->modulator_id == config.modulator_id
            && cfg->param_id == config.param_id;
    });
    modulation_configs_.erase(it);

    for (auto* l : modulation_listeners_) {
        l->OnModulationRemoved(modulator_id, param_id);
    }
}
}