#include "ParamBox.h"

namespace mana {
ParamBox::ParamBox(ParamSection& bank, std::string_view id_section, std::string_view section_name) {
    std::ranges::for_each(bank.get_section(id_section), [this](std::shared_ptr<Parameter>& p) mutable {
        auto k = p->create_knob();
        m_knobs.emplace_back(std::move(k));
    });
    m_gourp_box.SetText(section_name.data());
}

void ParamBox::show() {
    for (auto& pk : m_knobs) {
        pk->display();
    }
    m_gourp_box.Show();
}

void ParamBox::set_bounds(rgc::Bounds bounds) {
    m_gourp_box.set_bounds(bounds);

    auto b = m_gourp_box.get_bounds();
    for (int i = 0; auto & pk : m_knobs) {
        pk->set_bound(b.GetX() + 50 * i, b.GetY(), 50, 70);
        ++i;
    }
}

void ParamBox::set_top_left(Vector2 top_left) {
    m_gourp_box.set_bounds(rgc::Bounds(top_left.x, top_left.y, m_knobs.size() * 50, 70));

    auto b = m_gourp_box.get_bounds();
    for (int i = 0; auto & pk : m_knobs) {
        pk->set_bound(b.GetX() + 50 * i, b.GetY(), 50, 70);
        ++i;
    }
}
}