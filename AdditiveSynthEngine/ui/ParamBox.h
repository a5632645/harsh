#pragma once
#include "engine/modulation/ParamBank.h"
#include "ui/WrapGroupBox.h"
#include "ui/Knob.h"

namespace mana {
class ParamBox {
public:
    ParamBox(ParamBank& bank, std::string_view id_section, std::string_view section_name);
    void show();
    void set_bounds(rgc::Bounds bounds);
    void set_top_left(Vector2 top_left);
private:
    WrapGroupBox m_gourp_box;
    std::vector<std::shared_ptr<Knob>> m_knobs;
};
}