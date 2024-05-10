#pragma once

#include <vector>
#include <memory>
#include "ui/WrapDropBox.h"
#include "engine/synth.h"
#include <unordered_map>

namespace mana {
class ModulationMatrixLayout {
public:
    ModulationMatrixLayout(Synth& synth);
    ~ModulationMatrixLayout();

    ModulationMatrixLayout(const ModulationMatrixLayout&) = delete;
    ModulationMatrixLayout& operator=(const ModulationMatrixLayout&) = delete;
    ModulationMatrixLayout(ModulationMatrixLayout&&) = delete;
    ModulationMatrixLayout& operator=(ModulationMatrixLayout&&) = delete;

    void Paint();
    void SetBounds(Rectangle bound);
private:
    Synth& synth_;
    Rectangle bound_{};
    WrapDropBox param_selector_;
    WrapDropBox section_selector_;
    WrapDropBox modulator_selector_;
    rgc::Button add_button_;

    class SingleModulationLayout;
    SingleModulationLayout* remove_layout_{};

    void OnAddClick();
    void CheckAndRemoveLayout();
    std::vector<std::string_view> modulator_ids_;
    std::vector<std::string_view> sections_;

    struct SplitParamId {
        std::string_view detail;
        std::string_view id;
    };
    std::unordered_map<std::string_view, std::vector<SplitParamId>> split_param_ids;

    size_t layout_count_{};
    std::vector<std::unique_ptr<SingleModulationLayout>> layouts_;
};
}