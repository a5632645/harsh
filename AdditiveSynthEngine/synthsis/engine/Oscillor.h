#pragma once

#include <vector>
#include "partials.h"
#include "SineBank.h"
#include "engine/oscillor_param.h"
#include <algorithm>
#include <memory>
#include "modulation/modulator_bank.h"
#include "resynthsis/resynthsis.h"
#include "standard/freq.h"
#include "standard/phase.h"
#include "filter/filter_wrap.h"
#include "effect/effect.h"
#include "dissonance/dissonance.h"
#include "timber/timber.h"
#include "timber/unison.h"

namespace mana {
class Synth;
}

namespace mana {
class Oscillor {
public:
    Oscillor(Synth& synth);

    Oscillor(const Oscillor&) = delete;
    Oscillor& operator=(const Oscillor&) = delete;
    Oscillor(Oscillor&&) noexcept = default;
    Oscillor& operator=(Oscillor&&) noexcept = default;

    void Init(size_t bufferSize, float sampleRate, float update_rate, int update_skip);

    void NoteOn(int noteNumber, float v);

    bool IsPlaying() const {
        return note_on_;
    }

    int getMidiNote() const {
        return midi_note_;
    }

    void NoteOff();

    Partials& get_particles() { return partials_; }
    const Partials& GetPartials() const { return partials_; }
    decltype(auto) GetSmoothedGain() const { return sine_bank_.GetSmoothGainTable(); }

    Resynthesis& GetResynthsisProcessor() { return resynthsis_; }
    const Resynthesis& GetResynthsisProcessor() const { return resynthsis_; }

    void update_state(int skip);
    float SrTick() { return sine_bank_.SrTick(); }

    std::vector<std::string_view> GetModulatorIds() const { return modulator_bank_.GetModulatorsIds(); }
    std::vector<std::string_view> GetModulableParamIds() const { return oscillor_param_->GetParamIds(); }
    void CreateModulation(std::string_view param_id, std::string_view modulator_id, ModulationConfig* pconfig);
    void RemoveModulation(ModulationConfig& config);
private:
    SineBank sine_bank_;
    std::unique_ptr<OscillorParams> oscillor_param_;
    ModulatorBank modulator_bank_;
    Partials partials_;

    int midi_note_{ -1 };
    float midi_velocity_{};
    float sample_rate_{};
    bool note_on_ = false;

    FreqProcessor freq_;
    PhaseProcessor phase_;
    Timber timber_;
    Resynthesis resynthsis_;
    FilterWrap filter_;
    Effect effec0_;
    Effect effec1_;
    Effect effec2_;
    Effect effec3_;
    Effect effec4_;
    Dissonance dissonance_;
    Unison unison_;
};
}