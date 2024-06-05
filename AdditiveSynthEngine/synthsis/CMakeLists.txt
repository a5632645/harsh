project ("AdditiveSynthEngine")

if(WIN32)
    add_compile_options(/wd4819)
    add_compile_options("/arch:AVX")
    add_compile_options("/constexpr:steps2147483647")
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
"engine/modulation/curve_v2.cpp")
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
"engine/effect/chrous.h"
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
"utli/NoiseGenerator.h"
"engine/effect/decay.h"
"engine/effect/harmonic_delay.h"
#"engine/modulation/curve.h"
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
"utli/listener_list.h" "engine/modulation/curve_bank.h")

add_library(AdSynthLib STATIC ${ENGINE_SRC} ${ENGINE_HEADER})
target_link_libraries(AdSynthLib PUBLIC AudioFFT)
target_link_libraries(AdSynthLib PUBLIC AudioFile)
target_link_libraries(AdSynthLib PUBLIC gcem)
target_link_libraries(AdSynthLib PUBLIC xsimd)
target_link_libraries(AdSynthLib PUBLIC nlohmann_json::nlohmann_json)
target_include_directories(AdSynthLib PUBLIC ".")

set_property(TARGET AdSynthLib PROPERTY CXX_STANDARD 20)
