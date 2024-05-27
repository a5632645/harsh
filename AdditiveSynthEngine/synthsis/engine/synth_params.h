#pragma once

#include "engine/modulation/ParamBank.h"
#include "engine/modulation/Parameter.h"
#include "engine/modulation/curve.h"
#include "engine/resynthsis/resynthsis_data.h"
#include "engine/modulation/param_creator.h"
#include "engine/modulation/Modulation.h"

namespace mana {
class SynthParams {
public:
    SynthParams(std::shared_ptr<ParamCreator> creator);

    // ========================================================================================
    // param
    // ========================================================================================
    ParamBank& GetParamBank() { return param_bank_; }

    // ========================================================================================
    // curve
    // ========================================================================================
    CurveManager& GetCurveManager() { return curve_manager_; }

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

        virtual void OnModulationAdded(ModulationConfig& config) = 0;
        virtual void OnModulationRemoved(ModulationConfig& config) = 0;
    };
    void AddModulationListener(ModulationListener* listener) { modulation_listeners_.emplace_back(listener); }
    void RemoveModulationListener(ModulationListener* listener) { modulation_listeners_.erase(std::remove(modulation_listeners_.begin(), modulation_listeners_.end(), listener)); }

    std::shared_ptr<ModulationConfig> FindModulation(std::string_view modulator_id, std::string_view param_id);
    // notice: this function just add config
    void AddModulation(std::shared_ptr<ModulationConfig> config);
    void RemoveModulation(ModulationConfig& config);
    int GetModulationCount() const { return modulation_configs_.size(); }
    std::shared_ptr<ModulationConfig> GetModulation(int index) const { return modulation_configs_[index]; }
private:
    CurveManager curve_manager_;
    ParamBank param_bank_;
    ResynthsisFrames resynthsis_frames_;
    std::vector<std::shared_ptr<ModulationConfig>> modulation_configs_;
    std::vector<ModulationListener*> modulation_listeners_;
};
}