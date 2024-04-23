#include "raylib.h"
#include "Knob.h"
#include <algorithm>
#include <numbers>
#include <cmath>
#include <string>
#include <format>
#include "engine/modulation/Parameter.h"

namespace mana {
inline static std::shared_ptr<Parameter> s_empty_parameter = std::make_shared<Parameter>("", 0.0F);
}

namespace mana {
inline static Knob* s_currentKnob = nullptr;

Knob::Knob() {
    m_parameter = s_empty_parameter.get();
}

void Knob::display() {
    if (!enable_) {
        if (s_currentKnob == this) {
            s_currentKnob = nullptr;
        }
        return;
    }

    // logic
    auto const mousePosition = GetMousePosition();
    bool const isNowDown = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    if (isNowDown && m_isPressed && s_currentKnob == this) {
        auto const deltaY = m_lastMousePosition.y - mousePosition.y;
        m_counter += static_cast<int>(deltaY);
        auto const valueInc = m_counter / m_sensitivity;
        m_counter %= m_sensitivity;
        m_value += valueInc * m_step;
        m_lastMousePosition = mousePosition;
        auto newValue = std::clamp(m_value, m_min, m_max);
        m_value = newValue;
        m_parameter->set_current(m_value);
    }
    else if (isNowDown && !m_isPressed) {
        if (s_currentKnob == nullptr) {
            if (CheckCollisionPointRec(mousePosition, m_bounds)) {
                m_counter = 0;
                m_isPressed = true;
                m_lastMousePosition = mousePosition;
                s_currentKnob = this;
            }
        }
    }
    else if (!isNowDown && m_isPressed && s_currentKnob == this) {
        m_counter = 0;
        m_isPressed = false;
        s_currentKnob = nullptr;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && CheckCollisionPointRec(mousePosition, m_bounds)) {
        // set to default
        m_value = m_default_alue;
        m_counter = 0;
        m_isPressed = false;
        m_parameter->set_current(m_value);
    }

    // draw name
    {
        auto const numberTextWidth = MeasureText(m_name.data(), m_number_font_size);
        auto const exSpace = m_bounds.width - numberTextWidth;
        DrawText(m_name.data(), m_bounds.x + exSpace * 0.5F, m_bounds.y, m_name_font_size, m_fore_color);
    }

    Rectangle slider = {
        .x = m_bounds.x,
        .y = m_bounds.y + m_name_font_size,
        .width = m_bounds.width,
        .height = m_bounds.height - m_name_font_size - m_number_font_size
    };

    Rectangle text_bound = {
        .x = m_bounds.x,
        .y = m_bounds.y + m_bounds.height - m_number_font_size,
        .width = m_bounds.width,
        .height = static_cast<float>(m_number_font_size)
    };
    // draw number
    auto const numberText = value_to_text_function(m_value);
    auto const numberTextWidth = MeasureText(numberText.c_str(), m_number_font_size);
    auto const exSpace = text_bound.width - numberTextWidth;
    DrawText(numberText.c_str(), m_bounds.x + exSpace * 0.5F, text_bound.y, m_number_font_size, m_fore_color);

    // draw circle
    auto const centerX = slider.x + slider.width / 2.0F;
    auto const centerY = slider.y + slider.height / 2.0F;
    auto const radius = std::min(slider.width, slider.height) / 2.0F;
    DrawCircle(centerX, centerY, radius, m_bg_color);
    DrawCircleLines(centerX, centerY, radius, m_fore_color);

    // draw pointer
    {
        auto const pw = (m_value - m_min) / (m_max - m_min);
        auto const startAngle = 5.0F / 4.0F * std::numbers::pi_v<float>;
        auto const endAngle = -1.0F / 4.0F * std::numbers::pi_v<float>;
        auto const angle = startAngle + pw * (endAngle - startAngle);
        Vector2 const pointerPos = {
        .x = centerX + radius * std::cos(angle),
        .y = centerY - radius * std::sin(angle)
        };
        DrawLine(centerX, centerY, pointerPos.x, pointerPos.y, m_fore_color);
    }

    // draw pointer2
    if (m_parameter->has_modulation()) {
        auto const pw = (m_parameter->get_output_value() - m_min) / (m_max - m_min);
        auto const startAngle = 5.0F / 4.0F * std::numbers::pi_v<float>;
        auto const endAngle = -1.0F / 4.0F * std::numbers::pi_v<float>;
        auto const angle = startAngle + pw * (endAngle - startAngle);
        Vector2 const outer_pointer_pos = {
        .x = centerX + radius * 1.0F * std::cos(angle),
        .y = centerY - radius * 1.0F * std::sin(angle)
        };
        Vector2 const inner_pointer_pos = {
            .x = centerX + radius * 0.6F * std::cos(angle),
            .y = centerY - radius * 0.6F * std::sin(angle)
        };
        DrawLine(inner_pointer_pos.x, inner_pointer_pos.y, outer_pointer_pos.x, outer_pointer_pos.y, GREEN);
    }
}

Knob& Knob::set_title(std::string_view name) {
    m_name = name;
    return *this;
}

Knob& Knob::set_range(float min, float max, float step, float default_value) {
    m_min = min;
    m_max = max;
    m_step = step;
    m_default_alue = default_value;

    auto num = (m_max - m_min) / m_step;
    if (num > 400) {
        m_sensitivity = 1;
    }
    else if (num > 100) {
        m_sensitivity = 2;
    }
    else if (num > 80) {
        m_sensitivity = 3;
    }
    else if (num > 50) {
        m_sensitivity = 4;
    }
    else if (num > 20) {
        m_sensitivity = 8;
    }
    else {
        m_sensitivity = 50;
    }
    return *this;
}

Knob& Knob::set_sensitivity(int sensitivity) {
    m_sensitivity = sensitivity;
    return *this;
}

Knob& Knob::set_fore_color(Color fore_color) {
    m_fore_color = fore_color;
    return *this;
}

Knob& Knob::set_bg_color(Color bg_color) {
    m_bg_color = bg_color;
    return *this;
}

Knob& Knob::set_name_font_size(int name_font_size) {
    m_name_font_size = name_font_size;
    return *this;
}

Knob& Knob::set_number_font_size(int number_font_size) {
    m_number_font_size = number_font_size;
    return *this;
}

Knob& Knob::set_value(float v) {
    m_value = std::clamp(v, m_min, m_max);
    return *this;
}

Knob& Knob::set_bound(Rectangle bound) {
    m_bounds = bound;
    return *this;
}

Knob& Knob::set_bound(int x, int y, int w, int h) {
    m_bounds.x = x;
    m_bounds.y = y;
    m_bounds.width = w;
    m_bounds.height = h;
    return *this;
}

Knob& Knob::set_parameter(Parameter* parameter) {
    m_parameter = parameter;
    this->set_range(0.0f, 1.0f, 0.005f, parameter->get_default_value());
    this->set_value(parameter->get_default_value());
    return *this;
}

float Knob::get_value() const {
    return m_value;
}

void Knob::SetEnable(bool enable) {
    enable_ = enable;
}
}