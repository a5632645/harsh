#include "engine/synth.h"

#include <format>
#include "engine/oscillor_param.h"

namespace mana {
void Synth::BindParam() {
    param_bank_.AddOrCreateIfNull("standard.pitch_bend");
    param_bank_.AddOrCreateIfNull("standard.output_gain");

    param_bank_.AddOrCreateIfNull<IntParameter>("standard.phase.type");
    for (int arg_idx = 0; arg_idx < 2; ++arg_idx) {
        param_bank_.AddOrCreateIfNull(std::format("standard.phase.arg{}", arg_idx));
    }

    for (int osc_idx = 0; osc_idx < 2; ++osc_idx) {
        param_bank_.AddOrCreateIfNull<IntParameter>(std::format("timber.osc{}.type", osc_idx));
        for (int arg_idx = 0; arg_idx < 4; ++arg_idx) {
            param_bank_.AddOrCreateIfNull(std::format("timber.osc{}.arg{}", osc_idx, arg_idx));
        }
    }
    param_bank_.AddOrCreateIfNull("timber.osc2_shift");
    param_bank_.AddOrCreateIfNull("timber.osc2_beating");
    param_bank_.AddOrCreateIfNull("timber.osc1_gain");
    param_bank_.AddOrCreateIfNull("timber.osc2_gain");

    param_bank_.AddOrCreateIfNull<BoolParameter>("dissonance.enable");
    param_bank_.AddOrCreateIfNull<IntParameter>("dissonance.type");
    for (int arg_idx = 0; arg_idx < 2; ++arg_idx) {
        param_bank_.AddOrCreateIfNull(std::format("dissonance.arg{}", arg_idx));
    }

    param_bank_.AddOrCreateIfNull<BoolParameter>("filter.enable");
    param_bank_.AddOrCreateIfNull<IntParameter>("filter.type");
    for (int arg_idx = 0; arg_idx < 6; ++arg_idx) {
        param_bank_.AddOrCreateIfNull(std::format("filter.arg{}", arg_idx));
    }

    param_bank_.AddOrCreateIfNull<BoolParameter>("resynthsis.enable");
    for (int arg_idx = 0; arg_idx < 7; ++arg_idx) {
        param_bank_.AddOrCreateIfNull(std::format("resynthsis.arg{}", arg_idx));
    }

    for (int effect_idx = 0; effect_idx < 5; ++effect_idx) {
        param_bank_.AddOrCreateIfNull<BoolParameter>(std::format("effect{}.enable", effect_idx));
        param_bank_.AddOrCreateIfNull<IntParameter>(std::format("effect{}.type", effect_idx));
        for (int arg_idx = 0; arg_idx < 6; ++arg_idx) {
            param_bank_.AddOrCreateIfNull(std::format("effect{}.arg{}", effect_idx, arg_idx));
        }
    }

    for (int lfo_idx = 0; lfo_idx < 8; ++lfo_idx) {
        param_bank_.AddOrCreateIfNull(std::format("lfo{}.rate", lfo_idx));
        param_bank_.AddOrCreateIfNull(std::format("lfo{}.start_phase", lfo_idx));
        param_bank_.AddOrCreateIfNull(std::format("lfo{}.level", lfo_idx));
        param_bank_.AddOrCreateIfNull<BoolParameter>(std::format("lfo{}.restart", lfo_idx));
        param_bank_.AddOrCreateIfNull<IntParameter>(std::format("lfo{}.wave_type", lfo_idx));
        param_bank_.AddOrCreateIfNull<IntParameter>(std::format("lfo{}.wave_curve_idx", lfo_idx));
    };
}

void OscillorParams::ConnectSynthParams(const ParamBank& synth_params) {
    ConnectSingleSynthParams(synth_params, "standard.pitch_bend", &standard.pitch_bend);
    ConnectSingleSynthParams(synth_params, "standard.output_gain", &standard.output_gain);

    phase.phase_type = synth_params.GetParamPtr<IntParameter>("standard.phase.type");
    for (int arg_idx = 0; arg_idx < 2; ++arg_idx) {
        ConnectSingleSynthParams(synth_params, std::format("standard.phase.arg{}", arg_idx), phase.args.data() + arg_idx);
    }

    for (int osc_idx = 0; osc_idx < 2; ++osc_idx) {
        timber.osc_args[osc_idx].timber_type = synth_params.GetParamPtr<IntParameter>(std::format("timber.osc{}.type", osc_idx));
        for (int arg_idx = 0; arg_idx < 4; ++arg_idx) {
            ConnectSingleSynthParams(synth_params, std::format("timber.osc{}.arg{}", osc_idx, arg_idx), timber.osc_args[osc_idx].args.data() + arg_idx);
        }
    }
    ConnectSingleSynthParams(synth_params, "timber.osc2_shift", &timber.osc2_shift);
    ConnectSingleSynthParams(synth_params, "timber.osc2_beating", &timber.osc2_beating);
    ConnectSingleSynthParams(synth_params, "timber.osc1_gain", &timber.osc1_gain);
    ConnectSingleSynthParams(synth_params, "timber.osc2_gain", &timber.osc2_gain);

    dissonance.is_enable = synth_params.GetParamPtr<BoolParameter>("dissonance.enable");
    dissonance.dissonance_type = synth_params.GetParamPtr<IntParameter>("dissonance.type");
    for (int arg_idx = 0; arg_idx < 2; ++arg_idx) {
        ConnectSingleSynthParams(synth_params, std::format("dissonance.arg{}", arg_idx), dissonance.args.data() + arg_idx);
    }

    filter.is_enable = synth_params.GetParamPtr<BoolParameter>("filter.enable");
    filter.filter_type = synth_params.GetParamPtr<IntParameter>("filter.type");
    for (int arg_idx = 0; arg_idx < 6; ++arg_idx) {
        ConnectSingleSynthParams(synth_params, std::format("filter.arg{}", arg_idx), filter.args.data() + arg_idx);
    }

    resynthsis.is_enable = synth_params.GetParamPtr<BoolParameter>("resynthsis.enable");
    for (int arg_idx = 0; arg_idx < 7; ++arg_idx) {
        ConnectSingleSynthParams(synth_params, std::format("resynthsis.arg{}", arg_idx), resynthsis.args.data() + arg_idx);
    }

    for (int effect_idx = 0; effect_idx < 5; ++effect_idx) {
        effects[effect_idx].is_enable = synth_params.GetParamPtr<BoolParameter>(std::format("effect{}.enable", effect_idx));
        effects[effect_idx].effect_type = synth_params.GetParamPtr<IntParameter>(std::format("effect{}.type", effect_idx));
        for (int arg_idx = 0; arg_idx < 6; ++arg_idx) {
            ConnectSingleSynthParams(synth_params, std::format("effect{}.arg{}", effect_idx, arg_idx), effects[effect_idx].args.data() + arg_idx);
        }
    }

    for (int lfo_idx = 0; lfo_idx < 8; ++lfo_idx) {
        lfos[lfo_idx].restart = synth_params.GetParamPtr<BoolParameter>(std::format("lfo{}.restart", lfo_idx));
        lfos[lfo_idx].wave_type = synth_params.GetParamPtr<IntParameter>(std::format("lfo{}.wave_type", lfo_idx));
        lfos[lfo_idx].wave_curve_idx = synth_params.GetParamPtr<IntParameter>(std::format("lfo{}.wave_curve_idx", lfo_idx));
        ConnectSingleSynthParams(synth_params, std::format("lfo{}.rate", lfo_idx), &lfos[lfo_idx].rate);
        ConnectSingleSynthParams(synth_params, std::format("lfo{}.start_phase", lfo_idx), &lfos[lfo_idx].start_phase);
        ConnectSingleSynthParams(synth_params, std::format("lfo{}.level", lfo_idx), &lfos[lfo_idx].level);
    }
}

void OscillorParams::ConnectSingleSynthParams(const ParamBank& synth_params, std::string id, SingleOscillorParam * address) {
    address->synth_param_ref = synth_params.GetParamPtr(id);
    this->oscillor_param_table[std::move(id)] = address;
}

void OscillorParams::UpdateParams() {
    for (auto&&[k, v] : oscillor_param_table) {
        v->ouput_value = v->synth_param_ref->Get();
        v->modulation_value = 0.0f;
    }

    for (auto& modulation : oscillor_modulations) {
        modulation.ApplyModulation();
    }
}

void OscillorParams::CreateModulation(std::string_view param_id, Modulator * pmodulator, ModulationConfig * pconfig) {
    auto* pparam = oscillor_param_table[std::string(param_id)];
    assert(pparam != nullptr);
    oscillor_modulations.emplace_back(pparam, pmodulator, pconfig);
}

void OscillorParams::RemoveModulation(ModulationConfig& config) {
    auto it = std::ranges::find_if(oscillor_modulations, [config](SingleOscillorParamModulation& m) {
        return m.config->modulator_id == config.modulator_id
            && m.config->param_id == config.param_id;
    });
    oscillor_modulations.erase(it);
}

std::vector<std::string_view> OscillorParams::GetParamIds() const {
    std::vector<std::string_view> ids;
    for (auto&& [k, v] : oscillor_param_table) {
        ids.emplace_back(k);
    }
    return ids;
}
}