
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

add_executable(AdSynth ${UI_SRC} ${UI_HEADER})
target_link_libraries(AdSynth PRIVATE raylib)
target_link_libraries(AdSynth PRIVATE raylib_cpp)
target_link_libraries(AdSynth PRIVATE raygui_cpp)
target_link_libraries(AdSynth PRIVATE rtmidi)
target_link_libraries(AdSynth PRIVATE AdSynthLib)
#target_link_libraries(AdditiveSynthEngine PRIVATE raygui)
set_property(TARGET AdSynth PROPERTY CXX_STANDARD 20)