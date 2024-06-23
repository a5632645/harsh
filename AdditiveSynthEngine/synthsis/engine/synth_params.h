#pragma once

#include <nlohmann/json_fwd.hpp>
#include "engine/modulation/ParamBank.h"
#include "engine/modulation/Parameter.h"
#include "engine/resynthsis/resynthsis_data.h"
#include "engine/modulation/param_creator.h"
#include "engine/modulation/Modulation.h"
#include "modulation/curve_bank.h"
#include "utli/listener_list.h"

namespace mana {
class SynthParams {
public:
    SynthParams(std::shared_ptr<ParamCreator> creator);

    // ========================================================================================
    // load/save
    // ========================================================================================
    nlohmann::json SaveState() const;
    void LoadState(const nlohmann::json& json);

    // ========================================================================================
    // param
    // ========================================================================================
    ParamBank& GetParamBank() { return param_bank_; }

    // ========================================================================================
    // curve
    // ========================================================================================
    CurveBank& GetCurveBank() { return curve_bank_; }

    // ========================================================================================
    // resynthsis
    // ========================================================================================
    void SetResynthsisFrames(ResynthsisFrames new_frame) { resynthsis_frames_ = std::move(new_frame); }
    ResynthsisFrames& GetResynthsisFrames() { return resynthsis_frames_; }

    // ========================================================================================
    // modulation
    // ========================================================================================
    class ModulationListener {
    public:
        virtual ~ModulationListener() = default;

        virtual void OnModulationAdded(std::shared_ptr<ModulationConfig> config) = 0;
        virtual void OnModulationRemoved(std::string_view modulator_id, std::string_view param_id) = 0;
        virtual void OnModulationCleared() = 0;
    };
    void AddModulationListener(ModulationListener* listener) { modulation_listeners_.AddListener(listener); }
    void RemoveModulationListener(ModulationListener* listener) { modulation_listeners_.RemoveListener(listener); }

    std::shared_ptr<ModulationConfig> FindModulation(std::string_view modulator_id, std::string_view param_id);
    auto FindModulation(std::string_view param_id) {
        return modulation_configs_ | std::views::filter([param_id](std::shared_ptr<ModulationConfig>& cfg) {
            return cfg->param_id == param_id;
        });
    }
    // notice: this function just add config
    void AddModulation(std::shared_ptr<ModulationConfig> config);
    void RemoveModulation(ModulationConfig& config);
    int GetModulationCount() const { return static_cast<int>(modulation_configs_.size()); }
    std::shared_ptr<ModulationConfig> GetModulation(int index) const { return modulation_configs_[index]; }
private:
    nlohmann::json SaveModulationConfigs() const;
    nlohmann::json SaveResynthsisFrames() const;
    void LoadModulationConfigs(const nlohmann::json& json);
    void LoadResynthsisFrames(const nlohmann::json& json);

    CurveBank curve_bank_;
    ParamBank param_bank_;
    ResynthsisFrames resynthsis_frames_;
    std::vector<std::shared_ptr<ModulationConfig>> modulation_configs_;
    utli::ListenerList<ModulationListener> modulation_listeners_;
};
}