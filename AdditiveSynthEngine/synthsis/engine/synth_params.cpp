#include "synth_params.h"

#include <nlohmann/json.hpp>
#include "param/standard_param.h"
#include "param/timber_param.h"
#include "param/dissonance_param.h"
#include "param/effect_param.h"
#include "param/envelop_param.h"
#include "param/lfo_param.h"
#include "param/filter_param.h"
#include "param/resynthsis_param.h"
#include "param/unison_param.h"
#include "param/multi_env_param.h"

namespace mana {
static constexpr ParamRange kUnitRange{ 0.0f,1.0f };
}

namespace mana {
inline static constexpr struct FloatTag {} floatparam;
inline static constexpr struct IntTag {} intparam;
inline static constexpr struct IntChoiceTag {} intchoiceparam;
inline static constexpr struct BoolTag {} boolparam;

namespace detail {
template<typename T, typename Tag>
inline static auto CreateParam(ParamCreator& c, ModulationType t, float blend, T param, std::string id, Tag) {
    if constexpr (std::same_as<Tag, FloatTag>) {
        return c.CreateFloatParameter({
        .type = t,
        .id = std::move(id),
        .name = std::string{T::kName},
        .vmin = T::kMin,
        .vmax = T::kMax,
        .vdefault = T::kDefault,
        .vblend = blend });
    }
    else if constexpr (std::same_as<Tag, IntTag>) {
        return c.CreateIntParameter({
        .type = t,
        .id = std::move(id),
        .name = std::string{T::kName},
        .vmin = T::kMin,
        .vmax = T::kMax,
        .vdefault = T::kDefault
        .vblend = blend });
    }
    else if constexpr (std::same_as<Tag, BoolTag>) {
        return c.CreateBoolParameter({
        .id = std::move(id),
        .name = std::string{T::kName},
        .vdefault = T::kDefault });
    }
    else if constexpr (std::same_as<Tag, IntChoiceTag>) {
        return c.CreateIntChoiceParameter({
        .id = std::move(id),
        .name = std::string{T::kName},
        .vdefault = T::kDefault
        .choices = {T::kNames.begin(), T::kNames.end() } });
    }
}
}

template<typename T, typename Tag>
    requires requires {
    T::kId;
}
inline static auto CreateParam(Tag tag, ParamCreator& c, T param, ModulationType t = ModulationType::kDisable, float vblend = 1.0f) {
    return detail::CreateParam(c, t, vblend, param, std::string{ T::kId }, tag);
}
}

namespace mana {
SynthParams::SynthParams(std::shared_ptr<ParamCreator> creator) {
    using enum ModulationType;
    // ================================================================================
    // pitch and output
    // ================================================================================
    param_bank_.AddParameter(creator->CreateFloatParameter({
        .type = kDisable,
        .id = "bpm",
        .name = "bpm",
        .vmin = 0.0f,
        .vmax = 100000.0f,
        .vdefault = 120.0f }));
    param_bank_.AddParameter(creator->CreateFloatParameter({
        .type = kPoly,
        .id = "pitch_bend",
        .name = "pitch_bend",
        .vmin = param::PitchBend::kMin,
        .vmax = param::PitchBend::kMax,
        .vdefault = param::PitchBend::kDefault }));
    param_bank_.AddParameter(creator->CreateFloatParameter({
        .type = kDisable,
        .id = "output_gain",
        .name = "output_gain",
        .vmin = param::OutputGain::kMin,
        .vmax = param::OutputGain::kMax,
        .vdefault = param::OutputGain::kDefault }));

    // ================================================================================
    // phase
    // ================================================================================
    param_bank_.AddParameter(creator->CreateIntChoiceParameter({
        .id = "phase.type",
        .name = "phase.type",
        .choices = {param::PhaseType::kNames.begin(), param::PhaseType::kNames.end()},
        .vdefault = 0 }));
    for (int arg_idx = 0; arg_idx < 2; ++arg_idx) {
        param_bank_.AddParameter(creator->CreateFloatParameter({
            .type = kPoly,
            .id = std::format("phase.arg{}", arg_idx),
            .name = std::format("phase.arg{}", arg_idx),
            .vmin = 0.0f,
            .vmax = 1.0f,
            .vdefault = 0.0f }));
    }

    // ================================================================================
    // harmonic envelope
    // ================================================================================
    param_bank_.AddParameter(CreateParam(floatparam,
                                         *creator,
                                         param::VolEnv_PreDelay{},
                                         kPoly),
                             CreateParam(floatparam,
                                         *creator,
                                         param::VolEnv_Attack{},
                                         kPoly),
                             CreateParam(floatparam,
                                         *creator,
                                         param::VolEnv_Hold{},
                                         kPoly),
                             CreateParam(floatparam,
                                         *creator,
                                         param::VolEnv_Peak{},
                                         kPoly),
                             CreateParam(floatparam,
                                         *creator,
                                         param::VolEnv_Decay{},
                                         kPoly),
                             CreateParam(floatparam,
                                         *creator,
                                         param::VolEnv_Sustain{},
                                         kPoly),
                             CreateParam(floatparam,
                                         *creator,
                                         param::VolEnv_Release{},
                                         kPoly),
                             CreateParam(floatparam,
                                         *creator,
                                         param::VolEnv_HighScale{},
                                         kPoly));

    // ================================================================================
    // timber
    // ================================================================================
    for (int osc_idx = 0; osc_idx < 2; ++osc_idx) {
        param_bank_.AddParameter(creator->CreateIntChoiceParameter({
            .id = std::format("timber.osc{}.type", osc_idx),
            .name = std::format("timber.osc{}.type", osc_idx),
            .choices = {param::TimberType::kNames.begin(), param::TimberType::kNames.end()},
            .vdefault = 0 }));
        for (int arg_idx = 0; arg_idx < 4; ++arg_idx) {
            param_bank_.AddParameter(creator->CreateFloatParameter({
                .type = kPoly,
                .id = std::format("timber.osc{}.arg{}", osc_idx, arg_idx),
                .name = std::format("osc{}.arg{}", osc_idx, arg_idx),
                .vmin = 0.0f,
                .vmax = 1.0f,
                .vdefault = 0.0f }));
        }
    }
    param_bank_.AddParameter(creator->CreateFloatParameter({
        .type = kPoly,
        .id = "timber.osc2_shift",
        .name = "osc2_shift",
        .vmin = param::Timber_Osc2Shift::kMin,
        .vmax = param::Timber_Osc2Shift::kMax,
        .vdefault = param::Timber_Osc2Shift::kDefault }));
    param_bank_.AddParameter(creator->CreateFloatParameter({
        .type = kPoly,
        .id = "timber.osc2_beating",
        .name = "osc2_beating",
        .vmin = param::Timber_Osc2Beating::kMin,
        .vmax = param::Timber_Osc2Beating::kMax,
        .vdefault = param::Timber_Osc2Beating::kDefault }));
    param_bank_.AddParameter(creator->CreateFloatParameter({
        .type = kPoly,
        .id = "timber.osc1_gain",
        .name = "osc1_gain",
        .vmin = param::Timber_OscGain::kMin,
        .vmax = param::Timber_OscGain::kMax,
        .vdefault = param::Timber_OscGain::kDefault }));
    param_bank_.AddParameter(creator->CreateFloatParameter({
        .type = kPoly,
        .id = "timber.osc2_gain",
        .name = "osc2_gain",
        .vmin = param::Timber_OscGain::kMin,
        .vmax = param::Timber_OscGain::kMax,
        .vdefault = param::Timber_OscGain::kMin }));

    // ================================================================================
    // unison(alsing)
    // ================================================================================
    param_bank_.AddParameter(creator->CreateIntChoiceParameter({
        .id = "unison.type",
        .name = "unison.type",
        .choices = {param::Unison_Type::kNames.begin(), param::Unison_Type::kNames.end()},
        .vdefault = 0 }));
    param_bank_.AddParameter(creator->CreateIntParameter({
        .id = "unison.num_voice",
        .name = "unison.num_voice",
        .vmin = param::Unison_NumVoice::kMin,
        .vmax = param::Unison_NumVoice::kMax,
        .vdefault = param::Unison_NumVoice::kDefault }));
    param_bank_.AddParameter(creator->CreateFloatParameter({
        .type = kPoly,
        .id = "unison.pitch",
        .name = "unison.pitch",
        .vmin = param::Unison_Pitch::kMin,
        .vmax = param::Unison_Pitch::kMax,
        .vdefault = param::Unison_Pitch::kDefault,
        .vblend = 0.8f }));
    param_bank_.AddParameter(creator->CreateFloatParameter({
        .type = kPoly,
        .id = "unison.phase",
        .name = "unison.phase",
        .vmin = param::Unison_Phase::kMin,
        .vmax = param::Unison_Phase::kMax,
        .vdefault = param::Unison_Phase::kDefault }));
    param_bank_.AddParameter(creator->CreateFloatParameter({
        .type = kPoly,
        .id = "unison.pan",
        .name = "unison.pan",
        .vmin = param::Unison_Pan::kMin,
        .vmax = param::Unison_Pan::kMax,
        .vdefault = param::Unison_Pan::kDefault }));

    // ================================================================================
    // dissonance(inharmonic)
    // ================================================================================
    param_bank_.AddParameter(creator->CreateBoolParameter({
        .id = "dissonance.enable",
        .name = "dissonance.enable",
        .vdefault = false }));
    param_bank_.AddParameter(creator->CreateIntChoiceParameter({
        .id = "dissonance.type",
        .name = "dissonance.type",
        .choices = {param::DissonanceType::kNames.begin(), param::DissonanceType::kNames.end()},
        .vdefault = 0 }));
    for (int arg_idx = 0; arg_idx < 2; ++arg_idx) {
        param_bank_.AddParameter(creator->CreateFloatParameter({
            .type = kPoly,
            .id = std::format("dissonance.arg{}", arg_idx),
            .name = std::format("dissonance.arg{}", arg_idx),
            .vmin = 0.0f,
            .vmax = 1.0f,
            .vdefault = 0.0f }));
    }

    // ================================================================================
    // filter
    // ================================================================================
    param_bank_.AddParameter(creator->CreateBoolParameter({
        .id = "filter.stream_type",
        .name = "filter.stream_type",
        .vdefault = false }));
    for (int filter_idx = 0; filter_idx < 2; ++filter_idx) {
        param_bank_.AddParameter(creator->CreateBoolParameter({
            .id = std::format("filter{}.enable", filter_idx),
            .name = std::format("filter{}.enable", filter_idx),
            .vdefault = false }));
        param_bank_.AddParameter(creator->CreateIntChoiceParameter({
            .id = std::format("filter{}.type", filter_idx),
            .name = std::format("filter{}.type", filter_idx),
            .choices = {param::Filter_Type::kNames.begin(), param::Filter_Type::kNames.end()},
            .vdefault = 0 }));
        for (int arg_idx = 0; arg_idx < 8; ++arg_idx) {
            param_bank_.AddParameter(creator->CreateFloatParameter({
                .type = kPoly,
                .id = std::format("filter{}.arg{}", filter_idx, arg_idx),
                .name = std::format("filter{}.arg{}", filter_idx, arg_idx),
                .vmin = 0.0f,
                .vmax = 1.0f,
                .vdefault = 0.0f }));
        }
    }

    // ================================================================================
    // resynthsis
    // ================================================================================
    param_bank_.AddParameter(creator->CreateBoolParameter({
        .id = "resynthsis.enable",
        .name = "resynthsis.enable",
        .vdefault = false }));
    param_bank_.AddParameter(creator->CreateFloatParameter({
    .type = kPoly,
    .id = "resynthsis.freq_scale",
    .name = "resynthsis.freq_scale",
    .vmin = param::Resynthsis_FreqScale::kMin,
    .vmax = param::Resynthsis_FreqScale::kMax,
    .vdefault = param::Resynthsis_FreqScale::kDefault }));
    param_bank_.AddParameter(creator->CreateFloatParameter({
    .type = kPoly,
    .id = "resynthsis.start_offset",
    .name = "resynthsis.start_offset",
    .vmin = param::Resynthsis_FrameOffset::kMin,
    .vmax = param::Resynthsis_FrameOffset::kMax,
    .vdefault = param::Resynthsis_FrameOffset::kDefault }));
    param_bank_.AddParameter(creator->CreateFloatParameter({
    .type = kPoly,
    .id = "resynthsis.speed",
    .name = "resynthsis.speed",
    .vmin = param::Resynthsis_FrameSpeed::kMin,
    .vmax = param::Resynthsis_FrameSpeed::kMax,
    .vdefault = param::Resynthsis_FrameSpeed::kDefault }));
    param_bank_.AddParameter(creator->CreateFloatParameter({
    .type = kPoly,
    .id = "resynthsis.speedx",
    .name = "resynthsis.speedx",
    .vmin = param::Resynthsis_FrameSpeedMulti::kMin,
    .vmax = param::Resynthsis_FrameSpeedMulti::kMax,
    .vdefault = param::Resynthsis_FrameSpeedMulti::kDefault,
    .vblend = 0.3f }));
    param_bank_.AddParameter(creator->CreateFloatParameter({
    .type = kPoly,
    .id = "resynthsis.formant_mix",
    .name = "resynthsis.formant_mix",
    .vmin = param::Resynthsis_FormantMix::kMin,
    .vmax = param::Resynthsis_FormantMix::kMax,
    .vdefault = param::Resynthsis_FormantMix::kDefault }));
    param_bank_.AddParameter(creator->CreateFloatParameter({
    .type = kPoly,
    .id = "resynthsis.formant_shift",
    .name = "resynthsis.formant_shift",
    .vmin = param::Resynthsis_FormantShift::kMin,
    .vmax = param::Resynthsis_FormantShift::kMax,
    .vdefault = param::Resynthsis_FormantShift::kDefault }));
    param_bank_.AddParameter(creator->CreateFloatParameter({
    .type = kPoly,
    .id = "resynthsis.gain_mix",
    .name = "resynthsis.gain_mix",
    .vmin = param::Resynthsis_GainMix::kMin,
    .vmax = param::Resynthsis_GainMix::kMax,
    .vdefault = param::Resynthsis_GainMix::kDefault }));
    param_bank_.AddParameter(creator->CreateFloatParameter({
    .type = kDisable,
    .id = "resynthsis.start_range",
    .name = "resynthsis.start_range",
    .vmin = param::Resynthsis_StartRange::kMin,
    .vmax = param::Resynthsis_StartRange::kMax,
    .vdefault = param::Resynthsis_StartRange::kDefault,
    .vblend = 0.3f }));

    // ================================================================================
    // effect
    // ================================================================================
    for (int effect_idx = 0; effect_idx < 5; ++effect_idx) {
        param_bank_.AddParameter(creator->CreateBoolParameter({
            .id = std::format("effect{}.enable", effect_idx),
            .name = std::format("effect{}.enable", effect_idx),
            .vdefault = false }));
        param_bank_.AddParameter(creator->CreateIntChoiceParameter({
            .id = std::format("effect{}.type", effect_idx),
            .name = std::format("effect{}.type", effect_idx),
            .choices = {param::EffectType::kNames.begin(), param::EffectType::kNames.end()},
            .vdefault = 0 }));
        for (int arg_idx = 0; arg_idx < 6; ++arg_idx) {
            param_bank_.AddParameter(creator->CreateFloatParameter({
                .type = kPoly,
                .id = std::format("effect{}.arg{}", effect_idx, arg_idx),
                .name = std::format("effect{}.arg{}", effect_idx, arg_idx),
                .vmin = 0.0f,
                .vmax = 1.0f,
                .vdefault = 0.0f }));
        }
    }

    // ================================================================================
    // lfo
    // ================================================================================
    for (int lfo_idx = 0; lfo_idx < 5; ++lfo_idx) {
        param_bank_.AddParameter(
            creator->CreateIntChoiceParameter(
                {
                    .id = std::format(param::LFO_Mode::kIdFormater, lfo_idx),
                    .name = std::string{param::LFO_Mode::kName},
                    .choices = {param::LFO_Mode::kNames.begin(), param::LFO_Mode::kNames.end()},
                    .vdefault = 0
                }),
            creator->CreateIntChoiceParameter(
                {
                    .id = std::format(param::LFO_TimeType::kIdFormater, lfo_idx),
                    .name = std::string{param::LFO_TimeType::kName},
                    .choices = {param::LFO_TimeType::kNames.begin(), param::LFO_TimeType::kNames.end()},
                    .vdefault = 0
                }),
            creator->CreateIntChoiceParameter({
                .id = std::format(param::LFO_WaveType::kIdFormater, lfo_idx),
                .name = std::string{param::LFO_WaveType::kName},
                .choices = {param::LFO_WaveType::kNames.begin(), param::LFO_WaveType::kNames.end()},
                .vdefault = 0
                                              }),
            creator->CreateFloatParameter({
                .type = kPoly,
                .id = std::format(param::LFO_Phase::kIdFormater, lfo_idx),
                .name = std::string{param::LFO_Phase::kName},
                .vmin = param::LFO_Phase::kMin,
                .vmax = param::LFO_Phase::kMax,
                .vdefault = param::LFO_Phase::kDefault
                                          }),
            creator->CreateFloatParameter({
                .type = kPoly,
                .id = std::format("lfo{}.rate", lfo_idx),
                .name = "rate",
                .vmin = 0.0f,
                .vmax = 1.0f,
                .vdefault = 0.0f
                                          })
        );
    }

    // ================================================================================
    // envelop
    // ================================================================================
    for (int env_idx = 0; env_idx < 3; ++env_idx) {
        param_bank_.AddParameter(creator->CreateFloatParameter({
            .type = kPoly,
            .id = std::format("envelop{}.predelay", env_idx),
            .name = std::format("envelop{}.predelay", env_idx),
            .vmin = param::Env_Predelay::kMin,
            .vmax = param::Env_Predelay::kMax,
            .vdefault = param::Env_Predelay::kDefault }));
        param_bank_.AddParameter(creator->CreateFloatParameter({
            .type = kPoly,
            .id = std::format("envelop{}.attack", env_idx),
            .name = std::format("envelop{}.attack", env_idx),
            .vmin = param::Env_Attack::kMin,
            .vmax = param::Env_Attack::kMax,
            .vdefault = param::Env_Attack::kDefault }));
        param_bank_.AddParameter(creator->CreateFloatParameter({
            .type = kPoly,
            .id = std::format("envelop{}.hold", env_idx),
            .name = std::format("envelop{}.hold", env_idx),
            .vmin = param::Env_Hold::kMin,
            .vmax = param::Env_Hold::kMax,
            .vdefault = param::Env_Hold::kDefault }));
        param_bank_.AddParameter(creator->CreateFloatParameter({
            .type = kPoly,
            .id = std::format("envelop{}.decay", env_idx),
            .name = std::format("envelop{}.decay", env_idx),
            .vmin = param::Env_Decay::kMin,
            .vmax = param::Env_Decay::kMax,
            .vdefault = param::Env_Decay::kDefault }));
        param_bank_.AddParameter(creator->CreateFloatParameter({
            .type = kPoly,
            .id = std::format("envelop{}.sustain", env_idx),
            .name = std::format("envelop{}.sustain", env_idx),
            .vmin = param::Env_Sustain::kMin,
            .vmax = param::Env_Sustain::kMax,
            .vdefault = param::Env_Sustain::kDefault }));
        param_bank_.AddParameter(creator->CreateFloatParameter({
            .type = kPoly,
            .id = std::format("envelop{}.peak", env_idx),
            .name = std::format("envelop{}.peak", env_idx),
            .vmin = param::Env_Peak::kMin,
            .vmax = param::Env_Peak::kMax,
            .vdefault = param::Env_Peak::kDefault }));
        param_bank_.AddParameter(creator->CreateFloatParameter({
            .type = kPoly,
            .id = std::format("envelop{}.release", env_idx),
            .name = std::format("envelop{}.release", env_idx),
            .vmin = param::Env_Release::kMin,
            .vmax = param::Env_Release::kMax,
            .vdefault = param::Env_Release::kDefault }));
        param_bank_.AddParameter(creator->CreateFloatParameter({
            .type = kPoly,
            .id = std::format("envelop{}.att_exp", env_idx),
            .name = std::format("envelop{}.att_exp", env_idx),
            .vmin = -8.0f,
            .vmax = 8.0f,
            .vdefault = 0.0f }));
        param_bank_.AddParameter(creator->CreateFloatParameter({
            .type = kPoly,
            .id = std::format("envelop{}.dec_exp", env_idx),
            .name = std::format("envelop{}.dec_exp", env_idx),
            .vmin = -8.0f,
            .vmax = 8.0f,
            .vdefault = 0.0f }));
        param_bank_.AddParameter(creator->CreateFloatParameter({
            .type = kPoly,
            .id = std::format("envelop{}.rel_exp", env_idx),
            .name = std::format("envelop{}.rel_exp", env_idx),
            .vmin = -8.0f,
            .vmax = 8.0f,
            .vdefault = 0.0f }));
    }

    // ================================================================================
    // curves
    // ================================================================================
    curve_bank_.AddCurve(CurveV2{ kNumPartials, CurveV2::CurveInitEnum::kFull }, "resynthsis.rand_start_pos_mask")
        .AddCurve(CurveV2{ kNumPartials, CurveV2::CurveInitEnum::kFull }, "resynthsis.speed")
        .AddCurve(CurveV2{ kNumPartials, CurveV2::CurveInitEnum::kFull }, "effect.harmonic_delay.time")
        .AddCurve(CurveV2{ kNumPartials, CurveV2::CurveInitEnum::kFull }, "effect.harmonic_delay.feedback")
        .AddCurve("dissonance.prism")
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
    modulation_listeners_.CallListener(&ModulationListener::OnModulationAdded, config);
}

void SynthParams::RemoveModulation(ModulationConfig& config) {
    auto modulator_id = config.modulator_id;
    auto param_id = config.param_id;

    auto it = std::ranges::find_if(modulation_configs_, [config](std::shared_ptr<ModulationConfig>& cfg) {
        return cfg->modulator_id == config.modulator_id
            && cfg->param_id == config.param_id;
    });
    modulation_configs_.erase(it);
    modulation_listeners_.CallListener(&ModulationListener::OnModulationRemoved, modulator_id, param_id);
}

nlohmann::json SynthParams::SaveState() const {
    auto cb = curve_bank_.SaveState();
    auto pb = param_bank_.SaveState();
    auto mc = SaveModulationConfigs();
    auto rf = SaveResynthsisFrames();
    return nlohmann::json{
        {"curves", cb},
        {"params", pb},
        {"modulations", mc},
        {"resynthsis", rf}
    };
}

void SynthParams::LoadState(const nlohmann::json& json) {
    curve_bank_.LoadState(json["curves"]);
    param_bank_.LoadState(json["params"]);
    LoadModulationConfigs(json["modulations"]);
    LoadResynthsisFrames(json["resynthsis"]);
}

nlohmann::json SynthParams::SaveModulationConfigs() const {
    nlohmann::json out;
    for (const auto& pconfig : modulation_configs_) {
        out.push_back(nlohmann::json{
            {"modulator", pconfig->modulator_id},
            {"param", pconfig->param_id},
            {"amount", pconfig->amount},
            {"bipolar", pconfig->bipolar},
            {"enable", pconfig->enable},
                      });
    }
    return out;
}

void SynthParams::LoadModulationConfigs(const nlohmann::json& json) {
    modulation_configs_.clear();
    modulation_listeners_.CallListener(&ModulationListener::OnModulationCleared);
    for (const auto& pconfig : json) {
        AddModulation(std::make_shared<ModulationConfig>(
            pconfig["modulator"].get<std::string>(),
            pconfig["param"].get<std::string>(),
            pconfig["amount"].get<float>(),
            pconfig["bipolar"].get<bool>(),
            pconfig["enable"].get<bool>()));
    }
}

nlohmann::json SynthParams::SaveResynthsisFrames() const {
    // todo: complete
    return {};
}

void SynthParams::LoadResynthsisFrames(const nlohmann::json& json) {
    // todo: complete
}
}