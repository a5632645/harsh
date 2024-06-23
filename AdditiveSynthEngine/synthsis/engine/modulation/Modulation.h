#pragma once

#include <string>
#include <vector>

namespace mana {
struct ModulationConfig {
    struct Listener {
        virtual ~Listener() = default;

        virtual void OnConfigChanged(ModulationConfig* config) = 0;
    };
    std::vector<Listener*> listeners_;
    void AddListener(Listener* listener) {
        if (std::ranges::find(listeners_, listener) == listeners_.cend())
            listeners_.push_back(listener);
    }
    void RemoveListener(Listener* listener) {
        auto it = std::ranges::find(listeners_, listener);
        if (it != listeners_.cend())
            listeners_.erase(it);
    }

    void SetAmount(float v) {
        if (v == amount)
            return;

        amount = v;
        for (auto& l : listeners_) {
            l->OnConfigChanged(this);
        }
    }
    void SetEnable(bool v) {
        if (v == enable)
            return;

        enable = v;
        for (auto& l : listeners_) {
            l->OnConfigChanged(this);
        }
    }
    void SetBipolar(bool v) {
        if (v == bipolar)
            return;

        bipolar = v;
        for (auto& l : listeners_) {
            l->OnConfigChanged(this);
        }
    }

    ModulationConfig() = default;
    ModulationConfig(std::string modulator,
                     std::string param,
                     float amount,
                     bool bipolar,
                     bool enable)
        : modulator_id(std::move(modulator))
        , param_id(std::move(param))
        , amount(amount)
        , bipolar(bipolar)
        , enable(enable) {}

    std::string modulator_id;
    std::string param_id;
    float amount{};
    bool enable{};
    bool bipolar{};
};
}