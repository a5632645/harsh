#include "engine/synth.h"

#include "param/standard_param.h"
#include "param/dissonance_param.h"
#include "param/timber_param.h"
#include "param/filter_param.h"

namespace mana {
void Synth::BindParam() {
    param_bank_.AddOrCreateIfNull("standard.pitch_bend", param::PitchBend::GetNormalDefault())
        .on_output_changed = [this](float v) {synth_param_.standard.pitch_bend = v; };
    param_bank_.AddOrCreateIfNull("standard.output_gain", param::PitchBend::GetNormalDefault())
        .on_output_changed = [this](float v) {synth_param_.standard.output_gain = v; };

    for (int arg_idx = 0; arg_idx < 2; ++arg_idx) {
        param_bank_.AddOrCreateIfNull(std::format("standard.phase.arg{}", arg_idx), 0.0f)
            .on_output_changed = [this, arg_idx](float v) {synth_param_.phase.args[arg_idx] = v; };
    }

    for (int arg_idx = 0; arg_idx < 4; ++arg_idx) {
        param_bank_.AddOrCreateIfNull(std::format("timber.arg{}", arg_idx), 0.0f)
            .on_output_changed = [this, arg_idx](float v) {synth_param_.timber.args[arg_idx] = v; };
    }

    for (int arg_idx = 0; arg_idx < 2; ++arg_idx) {
        param_bank_.AddOrCreateIfNull(std::format("dissonance.arg{}", arg_idx), 0.0f)
            .on_output_changed = [this, arg_idx](float v) {synth_param_.dissonance.args[arg_idx] = v; };
    }

    for (int arg_idx = 0; arg_idx < 6; ++arg_idx) {
        param_bank_.AddOrCreateIfNull(std::format("filter.arg{}", arg_idx), 0.0f)
            .on_output_changed = [this, arg_idx](float v) {synth_param_.filter.args[arg_idx] = v; };
    }

    for (int arg_idx = 0; arg_idx < 7; ++arg_idx) {
        param_bank_.AddOrCreateIfNull(std::format("resynthsis.arg{}", arg_idx), 0.0f)
            .on_output_changed = [this, arg_idx](float v) {synth_param_.resynthsis.args[arg_idx] = v; };
    }

    for (int effect_idx = 0; effect_idx < 5; ++effect_idx) {
        for (int arg_idx = 0; arg_idx < 6; ++arg_idx) {
            param_bank_.AddOrCreateIfNull(std::format("effect{}.arg{}", effect_idx, arg_idx), 0.0f)
                .on_output_changed = [this, effect_idx, arg_idx](float v) {synth_param_.effects[effect_idx].args[arg_idx] = v; };
        }
    }
}
}