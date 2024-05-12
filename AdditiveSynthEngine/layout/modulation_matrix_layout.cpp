#include "modulation_matrix_layout.h"

#include "ui/wrap_check_box.h"
#include "ui/Knob.h"
#include "engine/modulation/Modulation.h"

namespace mana {
class ModulationMatrixLayout::SingleModulationLayout {
public:
    SingleModulationLayout(ModulationConfig& cfg) : config_ref_(cfg) {
        amount_.set_range(-1.0f, 1.0f, 0.0025f, 0.5f);
        amount_.set_title("amount");
        enable_.SetText("enable");
        bipolar_.SetText("bipolar");
        remove_.SetText("remove");
    }

    void Paint() {
        if (remove_.Show()) {
            on_remove_click(this);
        }
        enable_.Paint();
        bipolar_.Paint();
        amount_.display();

        config_ref_.enable = enable_.IsChecked();
        config_ref_.bipolar = bipolar_.IsChecked();
        config_ref_.amount = amount_.get_value();

        DrawText(config_ref_.modulator_id.data(), bound_.x + 300, bound_.y, 20, WHITE);
        DrawText(config_ref_.param_id.data(), bound_.x + 400, bound_.y, 20, WHITE);
    }

    void SetBounds(Rectangle bound) {
        bound_ = bound;
        enable_.SetBounds(rgc::Bounds(bound.x, bound.y, 16, 16));
        bipolar_.SetBounds(rgc::Bounds(bound.x + 100, bound.y, 16, 16));
        amount_.set_bound({ bound.x + 150,bound.y,50,50 });
        remove_.SetBounds(rgc::Bounds(bound.x + 200, bound.y, 50, 16));
    }

    ModulationConfig& GetModulationConfigRef() { return config_ref_; }

    std::function<void(SingleModulationLayout*)> on_remove_click = [](auto) {};
private:
    Rectangle bound_{};
    ModulationConfig& config_ref_;
    WrapCheckBox enable_;
    WrapCheckBox bipolar_;
    Knob amount_;
    rgc::Button remove_;
};

ModulationMatrixLayout::ModulationMatrixLayout(Synth& synth)
    : synth_(synth) {
    modulator_ids_ = synth.GetModulatorIds();
    auto param_ids = synth.GetModulableParamIds();

    // split param ids
    for (std::string_view id : param_ids) {
        auto sp_idx = id.find_first_of('.');
        auto section = id.substr(0, sp_idx);
        auto detail = id.substr(sp_idx + 1);
        split_param_ids[section].emplace_back(detail, id);
    }
    auto section_view = split_param_ids | std::ranges::views::keys;
    sections_.assign(section_view.begin(), section_view.end());
    section_selector_.SetChoices(section_view);
    section_selector_.on_choice_changed = [this](int c) {
        param_selector_.SetChoices(split_param_ids[sections_[c]] | std::views::transform([](SplitParamId& id) {
            return id.detail;
        }));
    };
    section_selector_.on_choice_changed(0);

    modulator_selector_.SetChoices(modulator_ids_);
    add_button_.SetText("add");
}

ModulationMatrixLayout::~ModulationMatrixLayout() = default;

void ModulationMatrixLayout::Paint() {
    if (add_button_.Show()) {
        OnAddClick();
    }

    for (auto& p : layouts_) {
        p->Paint();
    }

    param_selector_.Paint();
    modulator_selector_.Paint();
    section_selector_.Paint();

    CheckAndRemoveLayout();
}

void ModulationMatrixLayout::SetBounds(Rectangle bound) {
    bound_ = bound;
    section_selector_.SetBounds(rgc::Bounds(bound.x, bound.y, 100, 16));
    param_selector_.SetBounds(rgc::Bounds(bound.x + 100, bound.y, 100, 16));
    modulator_selector_.SetBounds(rgc::Bounds(bound.x + 200, bound.y, 50, 16));
    add_button_.SetBounds(rgc::Bounds(bound.x + 250, bound.y, 50, 16));

    for (int i = 0; auto & p : layouts_) {
        p->SetBounds({ bound.x, bound.y + 16 + 50 * i, 200,50 });
        ++i;
    }
}

void ModulationMatrixLayout::OnAddClick() {
    auto modulator_idx = modulator_selector_.get_item_selected();
    auto param_idx = param_selector_.get_item_selected();
    auto modulator_id = modulator_ids_[modulator_idx];
    auto section = sections_[section_selector_.get_item_selected()];
    auto param_id = split_param_ids[section][param_idx].id;

    auto&&[added, pconfig] = synth_.CreateModulation(modulator_id, param_id);
    if (!added) {
        return;
    }

    auto& l = layouts_.emplace_back(std::make_unique<SingleModulationLayout>(*pconfig));
    l->on_remove_click = [this](SingleModulationLayout* v) {remove_layout_ = v; };
    SetBounds(bound_);
}

void ModulationMatrixLayout::CheckAndRemoveLayout() {
    if (remove_layout_ == nullptr) {
        return;
    }

    auto it = std::ranges::find_if(layouts_, [this](std::unique_ptr<SingleModulationLayout>& p) {
        return p.get() == remove_layout_;
    });

    synth_.RemoveModulation(remove_layout_->GetModulationConfigRef());

    layouts_.erase(it);
    remove_layout_ = nullptr;
}
}