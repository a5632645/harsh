#pragma once

#include <vector>
#include "partials.h"
#include "SineBank.h"
#include "engine/oscillor_param.h"
#include <algorithm>
#include <memory>
#include "IProcessor.h"
#include "resynthsis/resynthsis.h"
#include "modulation/modulator_bank.h"

namespace mana {
class Synth;
}

namespace mana {
class Oscillor {
public:
    Oscillor(Synth& synth);

    void Init(size_t bufferSize, float sampleRate, float update_rate);

    void NoteOn(int noteNumber, float v);

    bool canPlayNote(int note) const {
        if (midi_note_ == -1) return true;
        return false;
    }

    bool IsPlaying() const {
        return true;
        return midi_note_ != -1;
    }

    int getMidiNote() const {
        return midi_note_;
    }

    void NoteOff() {
        midi_note_ = -1;
        std::ranges::for_each(processors_, [=](std::shared_ptr<IProcessor>& p) {p->OnNoteOff(); });
        modulator_bank_.OnNoteOff();
    }

    Partials& get_particles() { return partials_; }
    const Partials& GetPartials() const { return partials_; }

    Resynthesis& GetResynthsisProcessor() { return *p_resynthsis_; }
    const Resynthesis& GetResynthsisProcessor() const { return *p_resynthsis_; }

    void update_state(int skip);
    float SrTick() { return sine_bank_.SrTick(); }

    std::vector<std::string_view> GetModulatorIds() const { return modulator_bank_.GetModulatorsIds(); }
    std::vector<std::string_view> GetModulableParamIds() const { return oscillor_param_.GetParamIds(); }
    void CreateModulation(std::string_view param_id, std::string_view modulator_id, ModulationConfig* pconfig);
    void RemoveModulation(ModulationConfig& config);
private:
    IProcessor* AddProcessor(std::shared_ptr<IProcessor>&& processor) {
        return processors_.emplace_back(std::move(processor)).get();
    }

    SineBank sine_bank_;
    OscillorParams oscillor_param_;
    ModulatorBank modulator_bank_;
    Partials partials_;

    int midi_note_{ -1 };
    float midi_velocity_{};
    float sample_rate_{};
    bool note_on_ = false;

    // component
    std::vector<std::shared_ptr<IProcessor>> processors_;
    Resynthesis* p_resynthsis_{};
};
}