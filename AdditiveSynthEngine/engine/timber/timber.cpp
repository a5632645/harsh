#include "timber.h"

#include "param/param.h"

namespace mana {
void Timber::Init(float sample_rate) {
    dual_saw_.Init(sample_rate);
    sync_.Init(sample_rate);
}

void Timber::Process(Partials& partials) {
    using enum param::TimberType::TimberEnum;

    switch (timber_type_) {
    case kDualSaw:
        dual_saw_.Process(partials);
        break;
    case kSync:
        sync_.Process(partials);
        break;
    }
}

void Timber::OnUpdateTick(const SynthParam& params, int skip) {
    timber_type_ = param::IntChoiceParam<param::TimberType, param::TimberType::TimberEnum>::GetEnum(
        params.timber.timber_type
    );

    dual_saw_.OnUpdateTick(params, skip);
    sync_.OnUpdateTick(params, skip);
}

void Timber::OnNoteOn(int note) {
    dual_saw_.OnNoteOn(note);
    sync_.OnNoteOn(note);
}

void Timber::OnNoteOff() {
}
}