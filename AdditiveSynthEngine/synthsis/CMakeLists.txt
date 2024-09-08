project ("AdditiveSynthEngine")

if(MSVC)
    add_compile_options(/wd4819)
    add_compile_options("/arch:AVX")
    add_compile_options("/constexpr:steps2147483647")
else()
    add_compile_options("-mavx")
endif()

option(USE_XSIMD "Use xsimd" OFF)
if(${USE_XSIMD})
    add_compile_definitions(AD_ENABLE_SIMD)
endif()

set(ENGINE_SRC
    "engine/Oscillor.cpp"
    "engine/synth.cpp"
    "engine/standard/freq.cpp"
    "engine/timber/timber.cpp"
    "engine/timber/dual_saw.cpp"
    "engine/timber/sync.cpp"
    "engine/filter/filter.cpp"
    "engine/dissonance/dissonance.cpp"
    "engine/resynthsis/resynthsis.cpp"
    "engine/standard/phase.cpp"
    "engine/timber/timber_gen.cpp"
    "engine/modulation/lfo.cpp"
    "engine/modulation/modulator_bank.cpp"
    "engine/oscillor_param.cpp"
    "engine/synth_params.cpp"
    "engine/timber/unison.cpp"
    "engine/timber/noise.cpp"
    "engine/modulation/envelop.cpp"
    "engine/filter/formant_filter.cpp"
    "engine/filter/filter_wrap.cpp"
    "engine/sine_bank.cpp"
    "engine/effect/effect.cpp"
    "engine/modulation/curve_v2.cpp"
    "engine/timber/multi_env.cpp"
    "engine/modulation/curve_bank.cpp"
    "engine/modulation/quantize_map.cpp"
    "engine/time_effect/delay.cpp"
    "engine/time_effect/distortion.cpp"
    "engine/time_effect/chrous.cpp"
    "engine/time_effect/reverb.cpp"
    "engine/time_effect/dsp/dsp/apf.c"
    "engine/time_effect/dsp/dsp/fdn_reverb.c"
    "engine/time_effect/dsp/dsp/freq_shift.c")
set(ENGINE_HEADER
    "engine/timber/sync.h"
    "engine/Oscillor.h"
    "engine/standard/freq.h"
    "engine/modulation/Parameter.h"
    "engine/modulation/ParamBank.h"
    "engine/modulation/Modulator.h"
    "engine/modulation/Modulation.h"
    "param/timber_param.h"
    "engine/timber/dual_saw.h"
    "param/param.h"
    "engine/oscillor_param.h"
    "param/standard_param.h"
    "param/dissonance_param.h"
    "engine/filter/filter.h"
    "engine/dissonance/dissonance.h"
    "param/effect_param.h"
    "engine/effect/octaver.h"
    "engine/effect/reverb.h"
    "engine/resynthsis/resynthsis.h"
    "engine/resynthsis/resynthsis_data.h"
    "engine/resynthsis/window.h"
    "engine/effect/phaser.h"
    "engine/effect/scramble.h"
    "param/resynthsis_param.h"
    "utli/spin_lock.h"
    "utli/convert.h"
    "engine/standard/phase.h"
    "utli/warp.h"
    "engine/timber/timber_frame.h"
    "engine/timber/timber_gen.h"
    "engine/effect/decay.h"
    "engine/effect/harmonic_delay.h"
    "engine/modulation/lfo.h"
    "param/lfo_param.h"
    "engine/modulation/modulator_bank.h"
    "engine/synth_params.h"
    "engine/poly_param.h"
    "engine/effect/effect_base.h"
    "engine/timber/osc_param.h"
    "engine/timber/unison.h"
    "engine/effect/spectral_gate.h"
    "engine/forward_decalre.h"
    "engine/timber/noise.h"
    "engine/timber/my_pwm.h"
    "engine/timber/impluse.h"
    "engine/filter/formant_filter.h"
    "engine/modulation/envelop.h"
    "param/envelop_param.h"
    "engine/modulation/param_range.h"
    "param/unison_param.h"
    "engine/filter/filter_wrap.h"
    "engine/modulation/param_creator.h"
    "engine/effect/effect.h"
    "param/param_helper.h" 
    "engine/resynthsis/image_base.h" 
    "engine/modulation/quantize_map.h" 
    "engine/modulation/curve_v2.h" 
    "utli/listener_list.h"
    "engine/modulation/curve_bank.h"
    "engine/timber/multi_env.h"
    "param/multi_env_param.h"
    "engine/DigitalResonator.h"
    "engine/time_effect/timefx_base.h"
    "engine/time_effect/fx_chain.h"
    "engine/time_effect/fx_chain.cpp"
    "engine/time_effect/delay.h"
    "engine/time_effect/reverb.h"
    "engine/time_effect/distortion.h"
    "engine/time_effect/chrous.h"
    "param/timefx/delay_param.h"
    "utli/smoother.h"
    "param/timefx/distortion_param.h"
    "engine/time_effect/dsp/statevar_filter.h"
    "engine/modulation/modu_type.h" "engine/time_effect/dsp/delayline.h" "param/timefx/chrous_param.h" "engine/time_effect/dsp/smooth_random.h" "engine/time_effect/dsp/corid_osc.h" "param/timefx/reverb_param.h")

add_library(AdSynthLib STATIC ${ENGINE_SRC} ${ENGINE_HEADER})
target_link_libraries(AdSynthLib PUBLIC AudioFFT)
target_link_libraries(AdSynthLib PUBLIC AudioFile)
target_link_libraries(AdSynthLib PUBLIC gcem)
target_link_libraries(AdSynthLib PUBLIC xsimd)
target_link_libraries(AdSynthLib PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(AdSynthLib PRIVATE freeverb)
target_include_directories(AdSynthLib PUBLIC ".")

set_property(TARGET AdSynthLib PROPERTY CXX_STANDARD 20)
