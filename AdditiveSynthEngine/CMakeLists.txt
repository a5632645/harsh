project ("AdditiveSynthEngine")
if(WIN32)
    add_compile_options(/wd4819)
    add_compile_options("/arch:AVX")
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
)
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
  "engine/modulation/curve.h"
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
  "param/param_helper.h")

set(UI_SRC
"AdditiveSynthEngine.cpp"
"ui/WrapDropBox.cpp"
"ui/Knob.cpp"
"layout/synth/dissonance_layout.cpp"
"layout/synth/timber_layout.cpp"
"ui/oscilloscope.cpp"
 "layout/synth/filter_layout.cpp"
 "layout/effect/sp_effect_layout.cpp"
 "layout/resynthsis/resynthsis_layout.cpp"
 "layout/standard_layout.cpp"
  "layout/synth/osc_layout.cpp"
  "ui/curve_editor.cpp"
  "layout/curve_layout.cpp"
  "layout/main_window.cpp"
  "ui/warp_text_input.cpp"
 "ui/wrap_check_box.cpp"
 "layout/modulators/lfo_layout.cpp"
 "layout/matrix/modulation_matrix_layout.cpp"
 "layout/synth/unison_layout.cpp"
 "ui/raygui.impl.cpp"
 "layout/modulators/envelop_layout.cpp"
 "layout/synth/synth_layout.cpp"
 "layout/synth/phase_layout.cpp"
 "layout/synth/filter_route_layout.cpp"
 "layout/synth/final_filter_layout.cpp"
 "layout/modulators/modulators_layout.cpp"
 "layout/effect/final_fx_layout.cpp"
 "layout/about/about_layout.cpp"
 "layout/master_layout.cpp"
 "ui/spectrum.cpp"
)
set(UI_HEADER
"layout/modulators/envelop_layout.h"
 "layout/synth/synth_layout.h"
 "layout/synth/phase_layout.h"
 "layout/synth/filter_route_layout.h"
 "layout/synth/final_filter_layout.h"
 "layout/modulators/modulators_layout.h"
 "layout/modulators/layout_base.h"
 "layout/effect/final_fx_layout.h"
 "layout/about/about_layout.h"
 "layout/master_layout.h"
 "ui/spectrum.h"
 "layout/synth/dissonance_layout.h"
"ui/WrapDropBox.h"
"layout/synth/timber_layout.h"
"ui/oscilloscope.h"
 "layout/synth/filter_layout.h"
 "layout/effect/sp_effect_layout.h"
 "layout/gui_param_pack.h"
 "layout/resynthsis/resynthsis_layout.h"
 "layout/standard_layout.h"
  "layout/synth/osc_layout.h"
    "ui/curve_editor.h"
  "layout/curve_layout.h"
  "layout/main_window.h"
  "ui/warp_text_input.h"
 "ui/wrap_check_box.h"
 "layout/modulators/lfo_layout.h"
 "layout/matrix/modulation_matrix_layout.h"
 "layout/synth/unison_layout.h"
)

add_library(AdSynthLib STATIC ${ENGINE_SRC} ${ENGINE_HEADER})
target_link_libraries(AdSynthLib PRIVATE AudioFFT)
target_link_libraries(AdSynthLib PRIVATE AudioFile)
target_link_libraries(AdSynthLib PRIVATE gcem)
target_link_libraries(AdSynthLib PUBLIC xsimd)
target_link_libraries(AdSynthLib PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(AdSynthLib PUBLIC ".")

add_executable(AdSynth ${UI_SRC} ${UI_HEADER})
target_link_libraries(AdSynth PRIVATE raylib)
target_link_libraries(AdSynth PRIVATE raylib_cpp)
target_link_libraries(AdSynth PRIVATE raygui_cpp)
target_link_libraries(AdSynth PRIVATE rtmidi)
target_link_libraries(AdSynth PRIVATE AdSynthLib)
#target_link_libraries(AdditiveSynthEngine PRIVATE raygui)

set_property(TARGET AdSynthLib PROPERTY CXX_STANDARD 20)
set_property(TARGET AdSynth PROPERTY CXX_STANDARD 20)

add_subdirectory("plugin")
add_subdirectory("test")
