#pragma once

#include <array>
#include <unordered_map>
#include <algorithm>
#include "engine/modulation/Parameter.h"
#include "engine/modulation/ParamBank.h"
#include "engine/modulation/Modulation.h"
#include "synth_params.h"
#include "poly_param.h"

namespace mana {
class Modulator;
}

namespace mana {
class ModulableParams {
public:
    ModulableParams(SynthParams& synth_param, std::vector<ModulationType> collect_type);

    ModulableParams(const ModulableParams&) = delete;
    ModulableParams& operator=(const ModulableParams&) = delete;

    void UpdateParams();
    void CreateModulation(Modulator* pmodulator, std::shared_ptr<ModulationConfig> pconfig);
    void RemoveModulation(std::string_view modulator_id, std::string_view param_id);
    void ClearModulations();
    std::vector<std::string_view> GetParamIds() const;

    template<IsParamter P, class...T> requires (sizeof...(T) >= 1)
        P* GetParam(std::format_string<T...> format_text, T&&... args) { return parent_synth_param_.GetParamBank().GetParamPtr<P>(format_text, std::forward<T>(args)...); }
    template<IsParamter P>
    P* GetParam(std::string_view id) { return parent_synth_param_.GetParamBank().GetParamPtr<P>(id); }

    template<class...T>
    ModuFloatParameter* GetModuFloatParam(std::format_string<T...> format_text, T&&... args) { return oscillor_param_table_.at(std::format(format_text, std::forward<T>(args)...)); }
    SynthParams& GetParentSynthParams() { return parent_synth_param_; }

    template<typename P, class... Args>
    auto* GetParamPtr(P p, Args&&... args) {
        return parent_synth_param_.GetParamBank().GetParameterPtr(p, std::forward<Args>(args)...);
    }
private:
    struct SingleOscillorParamModulation {
        ModuFloatParameter* target{};
        Modulator* modulator{};
        std::shared_ptr<ModulationConfig> config{};
    };

    SynthParams& parent_synth_param_;
    std::unordered_map<std::string_view, ModuFloatParameter*> oscillor_param_table_;
    std::vector<SingleOscillorParamModulation> oscillor_modulations_;
    std::vector<std::unique_ptr<ModuFloatParameter>> poly_modu_params_;
};
}