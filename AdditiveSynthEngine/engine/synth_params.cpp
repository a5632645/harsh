#include "synth_params.h"

#include "param/standard_param.h"
#include "param/timber_param.h"
#include "param/dissonance_param.h"
#include "param/effect_param.h"
#include "param/envelop_param.h"
#include "param/lfo_param.h"
#include "param/filter_param.h"
#include "param/resynthsis_param.h"
#include "param/unison_param.h"

namespace mana {
static constexpr ParamRange kUnitRange{ 0.0f,1.0f };
}

namespace mana {
SynthParams::SynthParams() {
    using enum ModulationType;

    //param_bank_.AddOrCreateIfNull(kPoly, param::PitchBend::CreateRange(), "pitch_bend")
    //    .value_to_text = param::PitchBend::GetTextValue;
    //param_bank_.AddOrCreateIfNull(kMono, param::OutputGain::CreateRange(), "output_gain")
    //    .value_to_text = param::OutputGain::GetTextValue;
    param_bank_.AddParameter(param::PitchBend::CreateParam(kPoly, "pitch_bend"));
    param_bank_.AddParameter(param::OutputGain::CreateParam(kDisable, "output_gain"));

    //param_bank_.AddOrCreateIfNull<IntChoiceParameter>(kDisable, kUnitRange, "phase.type");
    param_bank_.AddParameter(param::PhaseType::CreateParam(kDisable, "phase.type"));
    for (int arg_idx = 0; arg_idx < 2; ++arg_idx) {
        param_bank_.AddOrCreateIfNull(kPoly, kUnitRange, "", "phase.arg{}", arg_idx);
    }

    for (int osc_idx = 0; osc_idx < 2; ++osc_idx) {
        //param_bank_.AddOrCreateIfNull<IntChoiceParameter>(kDisable, kUnitRange, "", "timber.osc{}.type", osc_idx);
        param_bank_.AddParameter(param::TimberType::CreateParam(kDisable, "timber.osc{}.type", osc_idx));
        for (int arg_idx = 0; arg_idx < 4; ++arg_idx) {
            param_bank_.AddOrCreateIfNull(kPoly, kUnitRange, "", "timber.osc{}.arg{}", osc_idx, arg_idx);
        }
    }
    //param_bank_.AddOrCreateIfNull(kPoly, kUnitRange, "", "timber.osc2_shift");
    //param_bank_.AddOrCreateIfNull(kPoly, kUnitRange, "", "timber.osc2_beating");
    //param_bank_.AddOrCreateIfNull(kPoly, kUnitRange, "", "timber.osc1_gain");
    //param_bank_.AddOrCreateIfNull(kPoly, kUnitRange, "", "timber.osc2_gain");
    param_bank_.AddParameter(param::Timber_Osc2Shift::CreateParam(kPoly, "timber.osc2_shift"));
    param_bank_.AddParameter(param::Timber_Osc2Beating::CreateParam(kPoly, "timber.osc2_beating"));
    param_bank_.AddParameter(param::Timber_OscGain::CreateParam(kPoly, "timber.osc1_gain"));
    param_bank_.AddParameter(param::Timber_OscGain::CreateParam(kPoly, "timber.osc2_gain"));

    param_bank_.AddOrCreateIfNull<IntChoiceParameter>(kDisable, kUnitRange, "", "unison.type");
    //param_bank_.AddOrCreateIfNull<IntChoiceParameter>(kDisable, kUnitRange, "", "unison.num_voice");
    param_bank_.AddParameter(param::Unison_NumVoice::CreateParam(kDisable, "unison.num_voice"));
    param_bank_.AddParameter(param::Unison_Pitch::CreateParam(kPoly, "unison.pitch"));
    param_bank_.AddParameter(param::Unison_Pan::CreateParam(kPoly, "unison.phase"));
    param_bank_.AddParameter(param::Unison_Phase::CreateParam(kPoly, "unison.pan"));
    //param_bank_.AddOrCreateIfNull(kPoly, kUnitRange, "", "unison.pitch");
    //param_bank_.AddOrCreateIfNull(kPoly, kUnitRange, "", "unison.phase");
    //param_bank_.AddOrCreateIfNull(kPoly, kUnitRange, "", "unison.pan");

    param_bank_.AddOrCreateIfNull<BoolParameter>(kDisable, kUnitRange, "", "dissonance.enable");
    //param_bank_.AddOrCreateIfNull<IntChoiceParameter>(kDisable, kUnitRange, "", "dissonance.type");
    param_bank_.AddParameter(param::DissonanceType::CreateParam(kDisable, "dissonance.type"));
    for (int arg_idx = 0; arg_idx < 2; ++arg_idx) {
        param_bank_.AddOrCreateIfNull(kPoly, kUnitRange, "", "dissonance.arg{}", arg_idx);
    }

    param_bank_.AddOrCreateIfNull<BoolParameter>(kDisable, kUnitRange, "", "filter.enable");
    //param_bank_.AddOrCreateIfNull<IntChoiceParameter>(kDisable, kUnitRange, "", "filter.type");
    param_bank_.AddParameter(param::Filter_Type::CreateParam(kDisable, "filter.type"));
    for (int arg_idx = 0; arg_idx < 6; ++arg_idx) {
        param_bank_.AddOrCreateIfNull(kPoly, kUnitRange, "", "filter.arg{}", arg_idx);
    }
    //param_bank_.AddOrCreateIfNull<IntChoiceParameter>(kDisable, kUnitRange, "", "filter.reso.type");
    param_bank_.AddParameter(param::ResonanceType::CreateParam(kDisable, "filter.reso.type"));
    for (int arg_idx = 0; arg_idx < 6; ++arg_idx) {
        param_bank_.AddOrCreateIfNull(kPoly, kUnitRange, "", "filter.reso.arg{}", arg_idx);
    }

    // resynthsis
    param_bank_.AddOrCreateIfNull<BoolParameter>(kDisable, kUnitRange, "", "resynthsis.enable");
    param_bank_.AddOrCreateIfNull<BoolParameter>(kDisable, kUnitRange, "", "resynthsis.formant_remap");
    for (int arg_idx = 0; arg_idx < 7; ++arg_idx) {
        param_bank_.AddOrCreateIfNull(kPoly, kUnitRange, "", "resynthsis.arg{}", arg_idx);
    }

    // effect
    for (int effect_idx = 0; effect_idx < 5; ++effect_idx) {
        param_bank_.AddOrCreateIfNull<BoolParameter>(kDisable, kUnitRange, "", "effect{}.enable", effect_idx);
        //param_bank_.AddOrCreateIfNull<IntChoiceParameter>(kDisable, kUnitRange, "", "effect{}.type", effect_idx);
        param_bank_.AddParameter(param::EffectType::CreateParam(kDisable, "effect{}.type", effect_idx));
        for (int arg_idx = 0; arg_idx < 6; ++arg_idx) {
            param_bank_.AddOrCreateIfNull(kPoly, kUnitRange, "", "effect{}.arg{}", effect_idx, arg_idx);
        }
    }

    // lfo
    for (int lfo_idx = 0; lfo_idx < 8; ++lfo_idx) {
        //param_bank_.AddOrCreateIfNull(kPoly, kUnitRange, "", "lfo{}.rate", lfo_idx);
        //param_bank_.AddOrCreateIfNull(kPoly, kUnitRange, "", "lfo{}.start_phase", lfo_idx);
        //param_bank_.AddOrCreateIfNull(kPoly, kUnitRange, "", "lfo{}.level", lfo_idx);
        //param_bank_.AddOrCreateIfNull<IntChoiceParameter>(kDisable, kUnitRange, "", "lfo{}.wave_type", lfo_idx);
        param_bank_.AddOrCreateIfNull<BoolParameter>(kDisable, kUnitRange, "", "lfo{}.restart", lfo_idx);
        param_bank_.AddParameter(param::LFO_Rate::CreateParam(kPoly, "lfo{}.rate", lfo_idx));
        param_bank_.AddParameter(param::LFO_Phase::CreateParam(kPoly, "lfo{}.start_phase", lfo_idx));
        param_bank_.AddParameter(param::LFO_Level::CreateParam(kPoly, "lfo{}.level", lfo_idx));
        param_bank_.AddParameter(param::LFO_WaveType::CreateParam(kPoly, "lfo{}.wave_type", lfo_idx));
    };

    // envelop
    for (int env_idx = 0; env_idx < 8; ++env_idx) {
        //param_bank_.AddOrCreateIfNull(kPoly, kUnitRange, "", "envelop{}.attack", env_idx);
        //param_bank_.AddOrCreateIfNull(kPoly, kUnitRange, "", "envelop{}.decay", env_idx);
        //param_bank_.AddOrCreateIfNull(kPoly, kUnitRange, "", "envelop{}.sustain", env_idx);
        //param_bank_.AddOrCreateIfNull(kPoly, kUnitRange, "", "envelop{}.release", env_idx);
        param_bank_.AddParameter(param::Env_Attack::CreateParam(kPoly, "envelop{}.attack", env_idx));
        param_bank_.AddParameter(param::Env_Decay::CreateParam(kPoly, "envelop{}.decay", env_idx));
        param_bank_.AddParameter(param::Env_Release::CreateParam(kPoly, "envelop{}.sustain", env_idx));
        param_bank_.AddParameter(param::Env_Sustain::CreateParam(kPoly, "envelop{}.release", env_idx));
    };

    // custom curves
    curve_manager_.AddCurve(kNumPartials, "resynthsis.formant_remap")
        .AddCurve(kNumPartials, "resynthsis.pos_offset")
        .AddCurve(kNumPartials, "effect.harmonic_delay.time")
        .AddCurve(kNumPartials, "effect.harmonic_delay.feedback")
        .AddCurve({ 0.0f,1.0f / 11.0f,2.0f / 11.0f,3.0f / 11.0f,4.0f / 11.0f,5.0f / 11.0f,6.0f / 11.0f,7.0f / 11.0f,8.0f / 11.0f,9.0f / 11.0f,10.0f / 11.0f,11.0f / 11.0f }, "dissonance.pitch_quantize");
    for (int i = 0; i < 8; ++i) {
        curve_manager_.AddCurve(kNumPartials, "lfo{}.wave", i);
    }
}
}