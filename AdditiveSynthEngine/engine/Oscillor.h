#pragma once

#include <vector>
#include "partials.h"
#include "SineBank.h"
#include "param/synth_param.h"
#include <algorithm>
#include <memory>
#include "IProcessor.h"
#include "resynthsis/resynthsis.h"

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
    }

    void forceChangeNoteNumber(int note) {
        midi_note_ = note;
    }

    void SetPitchShift(float v) {
        pitch_shift_ = v;
    }

    void setPhaseShift(float v) {
        phase_shift_ = v;
    }

    Partials& get_particles() {
        return partials_;
    }

    const Partials& GetPartials() const {
        return partials_;
    }

    Resynthesis& GetResynthsisProcessor() { return *p_resynthsis_; }
    const Resynthesis& GetResynthsisProcessor() const { return *p_resynthsis_; }

    //=============================STREAM================================================
    void update_state(const SynthParam& param, int skip);

    void renderBuffer(size_t length) {
        std::ranges::fill(audio_buffer_, 0.0F);

        if (!IsPlaying()) return;

        for (size_t i = 0; i < length; ++i) {
            audio_buffer_[i] = sine_bank_.SrTick();
        }
    }

    float SrTick() { return sine_bank_.SrTick(); }

    const std::vector<float>& getBuffer() const {
        return audio_buffer_;
    }

private:
    IProcessor* AddProcessor(std::shared_ptr<IProcessor>&& processor) {
        return processors_.emplace_back(std::move(processor)).get();
    }

    std::vector<float> audio_buffer_;
    SineBank sine_bank_;
    Partials partials_;

    int midi_note_{ -1 };
    float midi_velocity_{};
    float sample_rate_{};
    float pitch_shift_{};
    float phase_shift_{};
    bool note_on_ = false;

    // component
    std::vector<std::shared_ptr<IProcessor>> processors_;
    Resynthesis* p_resynthsis_{};
};
}