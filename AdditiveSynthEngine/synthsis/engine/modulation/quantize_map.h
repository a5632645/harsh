#pragma once

#include <vector>

namespace mana {
class QuantizeMap {
public:
    struct Listener {
        virtual ~Listener() = default;
        virtual void OnReload(QuantizeMap* map) = 0;
        virtual void OnPointChanged(QuantizeMap* map, int idx) = 0;
    };

    void AddListener(Listener* l) {
        if (std::ranges::find(listeners_, l) == listeners_.cend())
            listeners_.push_back(l);
    }
    void RemoveListener(Listener* l) {
        auto it = std::remove(listeners_.begin(), listeners_.end(), l);
        if (it == listeners_.cend())
            return;
        listeners_.erase(it);
    }

    constexpr QuantizeMap() = default;
    constexpr QuantizeMap(int size) : data_(size) {}
    constexpr QuantizeMap(std::initializer_list<float> e) : data_(e) {}

    void Set(int idx, float new_val) {
        if (new_val == data_[idx])
            return;

        data_[idx] = new_val;
        for (auto* l : listeners_) {
            l->OnPointChanged(this, idx);
        }
    }

    float Get(int idx) const {
        return data_[idx];
    }

private:
    std::vector<Listener*> listeners_;
    std::vector<float> data_;
};
}