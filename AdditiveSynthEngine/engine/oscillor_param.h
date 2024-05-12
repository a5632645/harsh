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
class OscillorParams {
public:
    OscillorParams(SynthParams& synth_param);

    OscillorParams(const OscillorParams&) = delete;
    OscillorParams& operator=(const OscillorParams&) = delete;

    void ConnectSynthParam(SynthParams& synth_param);
    void UpdateParams();
    void CreateModulation(std::string_view param_id, Modulator* pmodulator, ModulationConfig* pconfig);
    void RemoveModulation(ModulationConfig& config);
    std::vector<std::string_view> GetParamIds() const;

    template<IsParamter P>
    P* GetParam(std::string_view id) { return parent_synth_param_.GetParamBank().GetParamPtr<P>(id); }
    template<IsParamter P, class...T> requires (sizeof...(T) >= 1)
        P* GetParam(std::format_string<T...> format_text, T&&... args) { return parent_synth_param_.GetParamBank().GetParamPtr<P>(format_text, std::forward<T>(args)...); }
    PolyModuFloatParameter* GetPolyFloatParam(std::string_view id) { return oscillor_param_table_.at(id); }
    template<class...T> requires (sizeof...(T) >= 1)
        PolyModuFloatParameter* GetPolyFloatParam(std::format_string<T...> format_text, T&&... args) { return oscillor_param_table_.at(std::format(format_text, std::forward<T>(args)...)); }
    SynthParams& GetParentSynthParams() { return parent_synth_param_; }

private:
    struct SingleOscillorParamModulation {
        PolyModuFloatParameter* target{};
        Modulator* modulator{};
        ModulationConfig* config{};
    };

    SynthParams& parent_synth_param_;
    std::unordered_map<std::string_view, PolyModuFloatParameter*> oscillor_param_table_;
    std::vector<SingleOscillorParamModulation> oscillor_modulations_;
    std::vector<std::unique_ptr<PolyModuFloatParameter>> poly_modu_params_;
};
}