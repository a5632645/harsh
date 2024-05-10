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
Oscillor::Oscillor(Synth& synth) {
    oscillor_param_.ConnectSynthParams(synth.GetParamBank());
    oscillor_param_.parent_synth = std::addressof(synth);

    AddProcessor(std::make_shared<FreqProcessor>());
    AddProcessor(std::make_shared<PhaseProcessor>());
    AddProcessor(std::make_shared<Dissonance>());
    AddProcessor(std::make_shared<Timber>());
    p_resynthsis_ = static_cast<Resynthesis*>(AddProcessor(std::make_shared<Resynthesis>(synth)));
    AddProcessor(std::make_shared<Filter>());
    AddProcessor(std::make_shared<Effect>(0));
    AddProcessor(std::make_shared<Effect>(1));
    AddProcessor(std::make_shared<Effect>(2));
    AddProcessor(std::make_shared<Effect>(3));
    AddProcessor(std::make_shared<Effect>(4));
}

void Oscillor::Init(size_t bufferSize, float sampleRate, float update_rate) {
    sample_rate_ = sampleRate;

    sine_bank_.Init(sampleRate);
    sine_bank_.SetSmoothTime(10.0F);
    sine_bank_.SetNumMaxActivePartials(kNumPartials);

    std::ranges::for_each(processors_, [=](std::shared_ptr<IProcessor>& p) {p->Init(sampleRate, update_rate); });
    modulator_bank_.Init(sampleRate, update_rate);
}

void Oscillor::NoteOn(int note_number, float velocity) {
    midi_velocity_ = velocity;
    midi_note_ = note_number;
    note_on_ = true;
    partials_.update_phase = true;
    sine_bank_.ResetState();

    std::ranges::for_each(processors_, [=](std::shared_ptr<IProcessor>& p) {p->OnNoteOn(note_number); });
    modulator_bank_.OnNoteOn(note_number);
}

void Oscillor::update_state(int step) {
    if (!IsPlaying()) return;

    modulator_bank_.OnUpdateTick(oscillor_param_, step, 0);
    oscillor_param_.UpdateParams();

    // first copy phase from sinebank
    std::ranges::copy(sine_bank_.GetPhaseTable(), partials_.phases.begin());

    // cr tick
    std::ranges::for_each(processors_, [=](std::shared_ptr<IProcessor>& p) {p->OnUpdateTick(oscillor_param_, step, 0); });

    // process by each processor
    static_cast<Resynthesis*>(p_resynthsis_)->PreGetFreqDiffsInRatio(partials_);
    std::ranges::for_each(processors_, [this](std::shared_ptr<IProcessor>& p) {p->Process(partials_); });

    // load into sinebank
    sine_bank_.LoadPartials(partials_);
}

void Oscillor::CreateModulation(std::string_view param_id, std::string_view modulator_id, ModulationConfig * pconfig) {
    auto* pmodulator = modulator_bank_.GetModulatorPtr(modulator_id);
    assert(pmodulator != nullptr);
    oscillor_param_.CreateModulation(param_id, pmodulator, pconfig);
}

void Oscillor::RemoveModulation(ModulationConfig& config) {
    oscillor_param_.RemoveModulation(config);
}
}