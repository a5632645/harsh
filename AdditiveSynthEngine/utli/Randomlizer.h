#pragma once
#include <random>
#include <concepts>

namespace mana {
class Randomlizer {
public:
    float next() {
        return static_cast<float>(m_random_device()) / static_cast<float>(m_random_device.max());
    }
private:
    std::random_device m_random_device;
};
}