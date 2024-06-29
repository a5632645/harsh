#pragma once

#include <string_view>
#include "engine/oscillor_param.h"

namespace mana {
class Modulator {
public:
    virtual ~Modulator() = default;
    Modulator(std::string_view id) : id_(id) {}

    virtual void Init(float sample_rate, float update_rate) = 0;
    virtual void PrepareParams(OscillorParams& params) = 0;
    virtual void OnUpdateTick() = 0;
    virtual void OnNoteOn(int note) = 0;
    virtual void OnNoteOff() = 0;

    float get_output_value() const { return output_value_; }
    void SetOutput(float v) { output_value_ = v; }

    std::string_view get_id() const { return id_; }

protected:
    std::string with_id(std::string_view x) const {
        return std::string(id_) + "_" + std::string(x);
    }

    std::string id_;
    float output_value_{};
};
}