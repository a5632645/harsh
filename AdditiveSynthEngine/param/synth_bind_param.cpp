#include "engine/synth.h"

namespace mana {
void Synth::BindParam() {
    param_bank_.AddOrCreateIfNull("voice.pitch_bend", "pitch_bend", 0.0f, -60.0f, 60.0f, 0.1f)
        .on_output_changed = [this](float v) {synth_param_.voice_param.pitch_bend = v; };

    param_bank_.AddOrCreateIfNull("timber.dual_saw.ratio", "ratio", 1.0f, 1.0f, 8.0f, 1.0f)
        .on_output_changed = [this](float v) {synth_param_.timber_param.dual_saw.ratio_ = v; };
    param_bank_.AddOrCreateIfNull("timber.dual_saw.beating_rate", "beating", 0.0f, -1.0f, 1.0f, 200)
        .on_output_changed = [this](float v) {synth_param_.timber_param.dual_saw.beating_rate_ = v; };
    param_bank_.AddOrCreateIfNull("timber.dual_saw.saw_square", "saw_square", 0.0f, 0.0f, 1.0f, 128)
        .on_output_changed = [this](float v) {synth_param_.timber_param.dual_saw.saw_square_ = v; };

    param_bank_.AddOrCreateIfNull("timber_proc.hard_sync.sync", "sync", 1.0f, 1.0f, 16.0f, 0.1f)
        .on_output_changed = [this](float v) {synth_param_.timber_proc_param.hard_sync.sync = v; };
}
}