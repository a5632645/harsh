# CMakeList.txt: AdditiveSynthEngine 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
include_directories(".")

# 将源代码添加到此项目的可执行文件.
add_executable (AdditiveSynthEngine
"AdditiveSynthEngine.cpp"
"engine/Oscillor.h"
"engine/Oscillor.cpp"
"engine/synth.cpp"

"engine/IProcessor.h"


"engine/standard/freq.h"
"engine/standard/freq.cpp"

"engine/modulation/Parameter.h"
"engine/modulation/Parameter.cpp"
"engine/modulation/ParamBank.h"
"engine/modulation/ParamBank.cpp"
"engine/modulation/Modulator.h"
"engine/modulation/Modulator.cpp"
"engine/modulation/Modulation.h"
"engine/modulation/Modulation.cpp"

"param/timber_param.h"
"engine/timber/timber.cpp"
"engine/timber/dual_saw.h"
"engine/timber/dual_saw.cpp"
"engine/timber/sync.h"
"engine/timber/sync.cpp"

"param/param.h"
"param/synth_param.h"
"param/synth_bind_param.cpp"
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
 "engine/effect/scramble.h" "param/resynthsis_param.h" "layout/resynthsis_layout.h" "layout/resynthsis_layout.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET AdditiveSynthEngine PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
target_link_libraries(AdditiveSynthEngine PUBLIC raylib)
target_link_libraries(AdditiveSynthEngine PUBLIC raygui_cpp)
target_link_libraries(AdditiveSynthEngine PUBLIC AudioFFT)
target_link_libraries(AdditiveSynthEngine PUBLIC AudioFile)
