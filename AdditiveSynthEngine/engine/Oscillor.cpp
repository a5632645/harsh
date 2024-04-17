#include "Oscillor.h"

#include "standard/freq.h"
#include "timber/timber.h"
#include "dissonance/dissonance.h"

namespace mana {
Oscillor::Oscillor() {
    AddProcessor(std::make_shared<FreqProcessor>());
    AddProcessor(std::make_shared<Dissonance>());
    AddProcessor(std::make_shared<Timber>());
}

void Oscillor::Init(size_t bufferSize, float sampleRate) {
    DSP_INIT;

    audio_buffer_.resize(bufferSize);
    sample_rate_ = sampleRate;

    sine_bank_.Init(sampleRate);
    sine_bank_.set_smooth_time(10.0F);
    sine_bank_.set_max_particles(kNumPartials);

    std::ranges::for_each(processors_, [=](std::shared_ptr<IProcessor>& p) {p->Init(sampleRate); });
}

void Oscillor::NoteOn(int note_number, float velocity) {
    midi_velocity_ = velocity;
    midi_note_ = note_number;
    note_on_ = true;
    partials_.update_phase = true;
    sine_bank_.note_on();

    std::ranges::for_each(processors_, [=](std::shared_ptr<IProcessor>& p) {p->OnNoteOn(note_number); });
}

void Oscillor::update_state(const SynthParam& param, int step) {
    if (!IsPlaying()) return;

    std::ranges::for_each(processors_, [=](std::shared_ptr<IProcessor>& p) {p->OnUpdateTick(param, step); });
    std::ranges::for_each(processors_, [this](std::shared_ptr<IProcessor>& p) {p->Process(partials_); });

    sine_bank_.load_particles(partials_);
}

void Oscillor::AddProcessor(std::shared_ptr<IProcessor> processor) {
    processors_.emplace_back(std::move(processor));
}
}