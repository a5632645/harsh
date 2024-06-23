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
            continue;
        }

        auto v = m.modulator->get_output_value();
        if (m.config->bipolar) {
            v = 2.0f * v - 1.0f;
        }
        m.target->modulation_value += v * m.config->amount;
    }
}

void OscillorParams::CreateModulation(Modulator* pmodulator, std::shared_ptr<ModulationConfig> pconfig) {
    auto* pparam = oscillor_param_table_[pconfig->modulator_id];
    assert(pparam != nullptr);
    oscillor_modulations_.emplace_back(pparam, pmodulator, pconfig);
}

void OscillorParams::RemoveModulation(std::string_view modulator_id, std::string_view param_id) {
    auto it = std::ranges::find_if(oscillor_modulations_, [modulator_id, param_id](SingleOscillorParamModulation& m) {
        return m.config->modulator_id == modulator_id
            && m.config->param_id == param_id;
    });
    oscillor_modulations_.erase(it);
}

void OscillorParams::ClearModulations() {
    oscillor_modulations_.clear();
}

std::vector<std::string_view> OscillorParams::GetParamIds() const {
    std::vector<std::string_view> ids;
    for (auto&& [k, v] : oscillor_param_table_) {
        ids.emplace_back(k);
    }
    return ids;
}
}