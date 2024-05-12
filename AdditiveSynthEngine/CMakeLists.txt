add_executable (AdditiveSynthEngine
"AdditiveSynthEngine.cpp"
"engine/Oscillor.h"
"engine/Oscillor.cpp"
"engine/synth.cpp"




"engine/standard/freq.h"
"engine/standard/freq.cpp"

"engine/modulation/Parameter.h"

"engine/modulation/ParamBank.h"

"engine/modulation/Modulator.h"

"engine/modulation/Modulation.h"


"param/timber_param.h"
"engine/timber/timber.cpp"
"engine/timber/dual_saw.h"
"engine/timber/dual_saw.cpp"
"engine/timber/sync.h"
"engine/timber/sync.cpp"

"param/param.h"
"engine/oscillor_param.h"

"param/standard_param.h"
"param/dissonance_param.h"

"param/filter_param.h"

"ui/WrapDropBox.h"
"ui/WrapDropBox.cpp"
#"ui/WrapGroupBox.h"
#"ui/WrapGroupBox.cpp"
"ui/Knob.cpp"



"layout/SynthLayout.h"
"layout/SynthLayout.cpp"
"layout/DissonanceLayout.h"
"layout/DissonanceLayout.cpp"
"layout/timber_layout.h"
"layout/timber_layout.cpp"

"ui/oscilloscope.h"
"ui/oscilloscope.cpp"
"engine/filter/filter.h"
"engine/filter/filter.cpp"
"engine/dissonance/dissonance.h"
"engine/dissonance/dissonance.cpp"


 "layout/filter_layout.h"
 "layout/filter_layout.cpp"
 "engine/effect/effect.h"
 "engine/effect/effect.cpp"
 "param/effect_param.h"
 "engine/effect/octaver.h"
 "layout/effect_layout.h"
 "layout/effect_layout.cpp"
 "engine/effect/reverb.h"
 "engine/effect/chrous.h"
 "engine/resynthsis/resynthsis.h"
 "engine/resynthsis/resynthsis.cpp"
 "engine/resynthsis/resynthsis_data.h"
 "engine/resynthsis/window.h"
 "layout/gui_param_pack.h"
 "engine/effect/phaser.h"
 "engine/effect/scramble.h"
 "param/resynthsis_param.h"
 "layout/resynthsis_layout.h"
 "layout/resynthsis_layout.cpp"
 "utli/NoiseGenerator.h"
 "layout/standard_layout.h"
 "layout/standard_layout.cpp"
  "utli/spin_lock.h"
  "utli/convert.h"
  "engine/standard/phase.h"
  "engine/standard/phase.cpp"
  "utli/warp.h"
  "engine/timber/timber_frame.h"
  "engine/timber/timber_gen.h"
  "engine/timber/timber_gen.cpp"
  "layout/osc_layout.h"
  "layout/osc_layout.cpp"
  
  "engine/effect/decay.h"
  "engine/effect/harmonic_delay.h"
  "engine/modulation/curve.h"
  "ui/curve_editor.h"
  "ui/curve_editor.cpp"
  "layout/curve_layout.h"
  "layout/curve_layout.cpp"
  "layout/main_window.h"
  "layout/main_window.cpp"
  "ui/warp_text_input.h"
  "ui/warp_text_input.cpp"
 "ui/wrap_check_box.h"
 "ui/wrap_check_box.cpp"
 "engine/modulation/lfo.h"
 "engine/modulation/lfo.cpp" "param/lfo_param.h" "engine/modulation/modulator_bank.h" "engine/modulation/modulator_bank.cpp" "layout/modulator_layout.h" "layout/modulator_layout.cpp" "layout/lfo_layout.h" "layout/lfo_layout.cpp" "layout/modulation_matrix_layout.h" "layout/modulation_matrix_layout.cpp" "engine/oscillor_param.cpp" "engine/synth_params.h" "engine/synth_params.cpp" "engine/poly_param.h" "engine/effect/effect_base.h" "engine/timber/osc_param.h"   "engine/timber/unison.h" "engine/timber/unison.cpp" "layout/unison_layout.h" "layout/unison_layout.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET AdditiveSynthEngine PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(AdditiveSynthEngine PUBLIC raylib)
target_link_libraries(AdditiveSynthEngine PUBLIC raylib_cpp)
target_link_libraries(AdditiveSynthEngine PUBLIC raygui_cpp)
target_link_libraries(AdditiveSynthEngine PUBLIC AudioFFT)
target_link_libraries(AdditiveSynthEngine PUBLIC AudioFile)
target_link_libraries(AdditiveSynthEngine PUBLIC gcem)
target_link_libraries(AdditiveSynthEngine PUBLIC xsimd)
target_link_libraries(AdditiveSynthEngine PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(AdditiveSynthEngine PRIVATE rtmidi)
target_include_directories(AdditiveSynthEngine PRIVATE .)