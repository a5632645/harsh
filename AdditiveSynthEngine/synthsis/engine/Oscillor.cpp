#include "Oscillor.h"

#include "standard/freq.h"
#include "standard/phase.h"
#include "timber/timber.h"
#include "dissonance/dissonance.h"
#include "resynthsis/resynthsis.h"
#include "filter/filter.h"
#include "effect/effect.h"
#include "engine/synth.h"

namespace mana {
Oscillor::Oscillor(Synth& synth)
    : resynthsis_(synth)
    , effec0_(0)
    , effec1_(1)
    , effec2_(2)
    , effec3_(3)
    , effec4_(4)
    , oscillor_param_(std::make_unique<OscillorParams>(synth.GetSynthParams())) {
    freq_.PrepareParams(*oscillor_param_);
    phase_.PrepareParams(*oscillor_param_);
    timber_.PrepareParams(*oscillor_param_);
    resynthsis_.PrepareParams(*oscillor_param_);
    filter_.PrepareParams(*oscillor_param_);
    effec0_.PrepareParams(*oscillor_param_);
    effec1_.PrepareParams(*oscillor_param_);
    effec2_.PrepareParams(*oscillor_param_);
    effec3_.PrepareParams(*oscillor_param_);
    effec4_.PrepareParams(*oscillor_param_);
    dissonance_.PrepareParams(*oscillor_param_);
    modulator_bank_.PrepareParams(*oscillor_param_);
    modulator_bank_.PrepareParams(*oscillor_param_);
    unison_.PrepareParams(*oscillor_param_);
    vol_env_.PrepareParam(*oscillor_param_);
}

void Oscillor::Init(size_t bufferSize, float sampleRate, float update_rate, int update_skip) {
    sample_rate_ = sampleRate;

    sine_bank_.Init(sampleRate, update_rate, update_skip);
    sine_bank_.SetNumMaxActivePartials(kNumPartials);

    freq_.Init(sampleRate, update_rate);
    phase_.Init(sampleRate, update_rate);
    timber_.Init(sampleRate, update_rate);
    resynthsis_.Init(sampleRate, update_rate);
    filter_.Init(sampleRate, update_rate);
    effec0_.Init(sampleRate, update_rate);
    effec1_.Init(sampleRate, update_rate);
    effec2_.Init(sampleRate, update_rate);
    effec3_.Init(sampleRate, update_rate);
    effec4_.Init(sampleRate, update_rate);
    dissonance_.Init(sampleRate, update_rate);
    modulator_bank_.Init(sampleRate, update_rate);
    modulator_bank_.Init(sampleRate, update_rate);
    unison_.Init(sampleRate, update_rate);
    vol_env_.Init(sampleRate, update_rate);
}

void Oscillor::NoteOn(int note_number, float velocity) {
    midi_velocity_ = velocity;
    midi_note_ = note_number;
    note_on_ = true;
    partials_.update_phase = true;
    sine_bank_.ResetState();

    freq_.OnNoteOn(note_number);
    phase_.OnNoteOn(note_number);
    timber_.OnNoteOn(note_number);
    resynthsis_.OnNoteOn(note_number);
    filter_.OnNoteOn(note_number);
    effec0_.OnNoteOn(note_number);
    effec1_.OnNoteOn(note_number);
    effec2_.OnNoteOn(note_number);
    effec3_.OnNoteOn(note_number);
    effec4_.OnNoteOn(note_number);
    dissonance_.OnNoteOn(note_number);
    modulator_bank_.OnNoteOn(note_number);
    modulator_bank_.OnNoteOn(note_number);
    unison_.OnNoteOn();
    vol_env_.OnNoteOn(note_number);
}

void Oscillor::NoteOff() {
    note_on_ = false;
    freq_.OnNoteOff();
    phase_.OnNoteOff();
    timber_.OnNoteOff();
    resynthsis_.OnNoteOff();
    filter_.OnNoteOff();
    effec0_.OnNoteOff();
    effec1_.OnNoteOff();
    effec2_.OnNoteOff();
    effec3_.OnNoteOff();
    effec4_.OnNoteOff();
    dissonance_.OnNoteOff();
    modulator_bank_.OnNoteOff();
    unison_.OnNoteOff();
    vol_env_.OnNoteOff();
}

void Oscillor::update_state(int step) {
    modulator_bank_.OnUpdateTick();
    oscillor_param_->UpdateParams();

    // first copy phase from sinebank
    std::ranges::copy(sine_bank_.GetPhaseTable(), partials_.phases.begin());
    vol_env_.OnUpdateTick();
    freq_.OnUpdateTick();
    phase_.OnUpdateTick();
    timber_.OnUpdateTick();
    resynthsis_.OnUpdateTick();
    filter_.OnUpdateTick();
    effec0_.OnUpdateTick();
    effec1_.OnUpdateTick();
    effec2_.OnUpdateTick();
    effec3_.OnUpdateTick();
    effec4_.OnUpdateTick();
    dissonance_.OnUpdateTick();
    unison_.OnUpdateTick();

    // process by each processor
    resynthsis_.PreGetFreqDiffsInRatio(partials_);
    freq_.Process(partials_);
    phase_.Process(partials_);
    dissonance_.Process(partials_);
    timber_.Process(partials_);
    resynthsis_.Process(partials_);
    vol_env_.Process(partials_);

    filter_.Process(partials_);
    effec0_.Process(partials_);
    effec1_.Process(partials_);
    effec2_.Process(partials_);
    effec3_.Process(partials_);
    effec4_.Process(partials_);

    unison_.Process(partials_);
    // load into sinebank
    sine_bank_.LoadPartials(partials_);
}

void Oscillor::Reset() {
    // todo: reset vol envelop and effect unit
}

void Oscillor::CreateModulation(std::string_view param_id, std::string_view modulator_id, ModulationConfig * pconfig) {
    auto* pmodulator = modulator_bank_.GetModulatorPtr(modulator_id);
    assert(pmodulator != nullptr);
    oscillor_param_->CreateModulation(param_id, pmodulator, pconfig);
}

void Oscillor::RemoveModulation(ModulationConfig& config) {
    oscillor_param_->RemoveModulation(config);
}
}