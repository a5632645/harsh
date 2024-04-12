#pragma once

#include <vector>
#include <string_view>
#include "Knob.h"

namespace mana {
class ChooseKnob : public Knob {
public:
    ChooseKnob();
    ~ChooseKnob() override = default;

    void add_choose(std::string_view name);
    void add_choose(std::vector<std::string_view> const& names);
private:
    std::string get_value_text() override;
    std::vector<std::string_view> m_values;
};


std::unique_ptr<ChooseKnob> create_choose_knob(std::string_view name,
                                               int x, int y, int w, int h,
                                               std::vector<std::string_view> values);

std::unique_ptr<ChooseKnob> create_choose_knob(int x, int y, int w, int h);
}