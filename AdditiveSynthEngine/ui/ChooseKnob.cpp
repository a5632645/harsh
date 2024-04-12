#include "ChooseKnob.h"
#include <cmath>
#include <iterator>
#include <vector>

namespace mana {
ChooseKnob::ChooseKnob() {
    set_sensitivity(50);
}

void ChooseKnob::add_choose(std::string_view name) {
    m_values.push_back(name);
    set_range(0.0F, m_values.size() - 1.0F, 1.0F, 0.0F);
}

void ChooseKnob::add_choose(std::vector<std::string_view> const& names) {
    std::ranges::copy(names, std::back_inserter(m_values));
    set_range(0.0F, m_values.size() - 1.0F, 1.0F, 0.0F);
}

std::string ChooseKnob::get_value_text() {
    auto index = static_cast<size_t>(std::round(m_value));
    return std::string(m_values[index]);
}

std::unique_ptr<ChooseKnob> create_choose_knob(std::string_view name,
                                               int x, int y, int w, int h,
                                               std::vector<std::string_view> values) {
    auto knob = std::make_unique<ChooseKnob>();
    knob->set_title(name);
    knob->set_bound(x, y, w, h);
    knob->add_choose(values);
    return knob;
}

std::unique_ptr<ChooseKnob> create_choose_knob(int x, int y, int w, int h) {
    auto knob = std::make_unique<ChooseKnob>();
    knob->set_bound(x, y, w, h);
    return knob;
}
}