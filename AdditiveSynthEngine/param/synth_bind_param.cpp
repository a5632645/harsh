#include "engine/synth.h"

#include "param/standard_param.h"
#include "param/dissonance_param.h"
#include "param/timber.h"
#include "param/filter_param.h"

namespace mana {
void Synth::BindParam() {
    param_bank_.AddOrCreateIfNull("standard.pitch_bend", param::FloatParam<param::PitchBend>::kNorDefault)
        .on_output_changed = [this](float v) {synth_param_.standard.pitch_bend = v; };

    param_bank_.AddOrCreateIfNull("timber.arg0", param::FloatParam<param::DualSaw_Ratio>::kNorDefault)
        .on_output_changed = [this](float v) {synth_param_.timber.arg0 = v; };
    param_bank_.AddOrCreateIfNull("timber.arg1", param::FloatParam<param::DualSaw_BeatingRate>::kNorDefault)
        .on_output_changed = [this](float v) {synth_param_.timber.arg1 = v; };
    param_bank_.AddOrCreateIfNull("timber.arg2", param::FloatParam<param::DualSaw_SawSquare>::kNorDefault)
        .on_output_changed = [this](float v) {synth_param_.timber.arg2 = v; };
    param_bank_.AddOrCreateIfNull("timber.arg3", 0.0f)
        .on_output_changed = [this](float v) {synth_param_.timber.arg3 = v; };

    param_bank_.AddOrCreateIfNull("dissonance.arg0", param::FloatParam<param::StringDissStretch>::kNorDefault)
        .on_output_changed = [this](float v) {synth_param_.dissonance.arg0 = v; };
    param_bank_.AddOrCreateIfNull("dissonance.arg1", 0.0f)
        .on_output_changed = [this](float v) {synth_param_.dissonance.arg1 = v; };

    param_bank_.AddOrCreateIfNull("filter.arg0", param::FloatParam<param::Filter_Cutoff>::kNorDefault)
        .on_output_changed = [this](float v) {synth_param_.filter.arg0 = v; };
    param_bank_.AddOrCreateIfNull("filter.arg1", param::FloatParam<param::Filter_Resonance>::kNorDefault)
        .on_output_changed = [this](float v) {synth_param_.filter.arg1 = v; };
    param_bank_.AddOrCreateIfNull("filter.arg2", param::FloatParam<param::Filter_Slope>::kNorDefault)
        .on_output_changed = [this](float v) {synth_param_.filter.arg2 = v; };
    param_bank_.AddOrCreateIfNull("filter.arg3", 0.0f)
        .on_output_changed = [this](float v) {synth_param_.filter.arg3 = v; };
    param_bank_.AddOrCreateIfNull("filter.arg4", 0.0f)
        .on_output_changed = [this](float v) {synth_param_.filter.arg4 = v; };
    param_bank_.AddOrCreateIfNull("filter.arg5", 0.0f)
        .on_output_changed = [this](float v) {synth_param_.filter.arg5 = v; };

    //param_bank_.AddOrCreateIfNull("timber_proc.hard_sync.sync", "sync", 1.0f, 1.0f, 16.0f, 0.1f)
    //    .on_output_changed = [this](float v) {synth_param_.timber_proc_param.hard_sync.sync = v; };

    /*param_bank_.AddOrCreateIfNull("voice.dissonance.string.strech", "strech", 0.0f, 0.0f, 0.1f, 1000)
        .on_output_changed = [this](float v) {synth_param_.voice_param.string_diss.strech = v; };*/
}
}