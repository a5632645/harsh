#include "oscillor_param.h"
#include "synth_params.h"
#include "engine/modulation/Modulator.h"

namespace mana {
OscillorParams::OscillorParams(SynthParams& synth_param)
    : parent_synth_param_(synth_param) {
    auto& bank = synth_param.GetParamBank();
    auto& synth_param_map = bank.GetParamMap<FloatParameter>();

    for (auto&&[k, v] : synth_param_map) {
        if (v->GetModulationType() == ModulationType::kPoly) {
            // add to oscillor params
            auto& p = poly_modu_params_.emplace_back(std::make_unique<PolyModuFloatParameter>(v.get()));
            oscillor_param_table_[v->GetId()] = p.get();
        }
    }
}

void OscillorParams::UpdateParams() {
    for (auto& p : poly_modu_params_) {
        p->modulation_value = 0.0f;
    }

    for (auto& m : oscillor_modulations_) {
        if (!m.config->enable) {
            return;
        }

        auto v = m.modulator->get_output_value();
        if (m.config->bipolar) {
            v = 2.0f * v - 1.0f;
        }
        m.target->modulation_value += v * m.config->amount;
    }
}

void OscillorParams::CreateModulation(std::string_view param_id, Modulator * pmodulator, ModulationConfig * pconfig) {
    auto* pparam = oscillor_param_table_[param_id];
    assert(pparam != nullptr);
    oscillor_modulations_.emplace_back(pparam, pmodulator, pconfig);
}

void OscillorParams::RemoveModulation(ModulationConfig& config) {
    auto it = std::ranges::find_if(oscillor_modulations_, [config](SingleOscillorParamModulation& m) {
        return m.config->modulator_id == config.modulator_id
            && m.config->param_id == config.param_id;
    });
    oscillor_modulations_.erase(it);
}

std::vector<std::string_view> OscillorParams::GetParamIds() const {
    std::vector<std::string_view> ids;
    for (auto&& [k, v] : oscillor_param_table_) {
        ids.emplace_back(k);
    }
    return ids;
}
}