#pragma once

#include <vector>
#include "partials.h"
#include "SineBank.h"
#include "utli/DebugMarco.h"
#include "param/synth_param.h"
#include <algorithm>
#include <memory>
#include "IProcessor.h"

namespace mana {
class Oscillor {
public:
    Oscillor();

    void Init(size_t bufferSize, float sampleRate);

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

    //=============================STREAM================================================
    void update_state(const SynthParam& param, int skip);

    void renderBuffer(size_t length) {
        std::ranges::fill(audio_buffer_, 0.0F);

        if (!IsPlaying()) return;

        for (size_t i = 0; i < length; ++i) {
            audio_buffer_[i] = sine_bank_.process_once();
        }
    }

    const std::vector<float>& getBuffer() const {
        return audio_buffer_;
    }

private:
    void AddProcessor(std::shared_ptr<IProcessor> processor);

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

    // Helper
    DSP_INIT_DEFINE;
};
}