#pragma once
#include <array>
#include <functional>
#include "raylib.h"
#include <iostream>

namespace mana {
class KeyBoard {
public:
    void checkInFrame() {
        if (IsKeyPressed(KEY_Z)) {
            m_currenctOctave = std::max(m_currenctOctave - 1, -2);
        }

        if (IsKeyPressed(KEY_X)) {
            m_currenctOctave = std::min(m_currenctOctave + 1, 8);
        }

        for (int i =0; const auto key : m_keys) {
            if (IsKeyPressed(key)) {
                onNoteOn(m_currenctOctave * 12 + i);
            }

            if (IsKeyReleased(key)) {
                onNoteOff(m_currenctOctave * 12 + i);
            }

            ++i;
        }
    }

    std::function<void(int)> onNoteOn;
    std::function<void(int)> onNoteOff;
private:
    int m_currenctOctave{4};
    inline static constexpr auto m_keys = std::to_array({ KEY_A, KEY_W, KEY_S, KEY_E, KEY_D, KEY_F, KEY_T, KEY_G, KEY_Y, KEY_H, KEY_U, KEY_J });
};
}