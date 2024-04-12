#pragma once

#include <functional>

namespace mana {
template<typename T>
class Value {
public:
    Value() : m_value(T{}) {
    }

    Value(
        T v
    ) :m_value(v) {
    }

    std::function<void(T)> callback = [](T) {};

    void set(T v) {
        if (m_value != v) {
            m_value = v;
            callback(m_value);
        }
    }

    T get() const {
        return m_value;
    }

    void operator=(T v) {
        set(v);
    }

    operator T() {
        return get();
    }

    T m_value{};
};
}