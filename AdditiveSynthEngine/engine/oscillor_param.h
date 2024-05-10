#pragma once

#include <array>
#include <unordered_map>
#include <algorithm>
#include "engine/modulation/Parameter.h"
#include "engine/modulation/ParamBank.h"
#include "engine/modulation/Modulator.h"
#include "engine/modulation/Modulation.h"

namespace mana {
class Synth;
}

namespace mana {
struct SingleOscillorParam {
    FloatParameter* synth_param_ref;
    float ouput_value{};
    float modulation_value{};

    float GetClamp() const { return std::clamp(ouput_value + modulation_value, 0.0f, 1.0f); }
};

using SingleIntOscillorParam = IntParameter*;
using SingleBoolOscillorParam = BoolParameter*;

struct SingleOscillorParamModulation {
    SingleOscillorParam* target{};
    Modulator* modulator{};
    ModulationConfig* config{};

    void ApplyModulation() {
        if (!config->enable) {
            return;
        }

        auto v = modulator->get_output_value();
        target->modulation_value += v * config->amount;
    }
};

struct OscillorParams {
    Synth* parent_synth;
    std::unordered_map<std::string, SingleOscillorParam*> oscillor_param_table;
    std::vector<SingleOscillorParamModulation> oscillor_modulations;

    void ConnectSynthParams(const ParamBank& synth_params);
    void ConnectSingleSynthParams(const ParamBank& synth_params, std::string id, SingleOscillorParam* address);
    void UpdateParams();
    void CreateModulation(std::string_view param_id, Modulator* pmodulator, ModulationConfig* pconfig);
    void RemoveModulation(ModulationConfig& config);
    std::vector<std::string_view> GetParamIds() const;

    struct StandardParams {
        SingleOscillorParam pitch_bend;
        SingleOscillorParam output_gain;
    } standard;

    struct PhaseParams {
        SingleIntOscillorParam phase_type;
        std::array<SingleOscillorParam, 2> args;
    } phase;

    struct OscParams {
        SingleIntOscillorParam timber_type;
        std::array<SingleOscillorParam, 4> args;
    };
    struct TimberParams {
        std::array<OscParams, 2> osc_args;
        SingleOscillorParam osc2_shift;
        SingleOscillorParam osc2_beating;
        SingleOscillorParam osc1_gain;
        SingleOscillorParam osc2_gain;
    } timber;

    struct DissParams {
        SingleBoolOscillorParam is_enable;
        SingleIntOscillorParam dissonance_type;
        std::array<SingleOscillorParam, 2> args;
    } dissonance;

    struct ResynthsisParams {
        SingleBoolOscillorParam is_enable;
        std::array<SingleOscillorParam, 7> args;
    } resynthsis;

    struct FilterParams {
        SingleBoolOscillorParam is_enable;
        SingleIntOscillorParam filter_type;
        std::array<SingleOscillorParam, 6> args;
    } filter;

    struct EffectParams {
        SingleBoolOscillorParam is_enable;
        SingleIntOscillorParam effect_type;
        std::array<SingleOscillorParam, 6> args;
    };
    std::array<EffectParams, 5 > effects;

    struct LfoParam {
        SingleOscillorParam rate;
        SingleOscillorParam start_phase;
        SingleOscillorParam level;
        SingleBoolOscillorParam restart;
        SingleIntOscillorParam wave_type;
        SingleIntOscillorParam wave_curve_idx;
    };
    std::array<LfoParam, 8> lfos;
};
}