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

"engine/modulation/Parameter.h"
"engine/modulation/Parameter.cpp"
"engine/modulation/ParamBank.h"
"engine/modulation/ParamBank.cpp"
"engine/modulation/Modulator.h"
"engine/modulation/ChoiceParameter.h"
"engine/modulation/ChoiceParameter.cpp"
"engine/modulation/Modulator.cpp"
"engine/modulation/Modulation.h"
"engine/modulation/Modulation.cpp"

"engine/IProcessor.h"
"engine/freq/freq.h"
"engine/freq/freq.cpp"
"engine/timber/dual_saw.h"
"engine/timber/dual_saw.cpp"

"param/synth_bind_param.cpp"
"param/voice_param.h"
"param/timber_param.h"
"param/synth_param.h"

#"ui/WrapDropBox.h"
#"ui/WrapDropBox.cpp"
#"ui/WrapGroupBox.h"
#"ui/WrapGroupBox.cpp"
#"ui/ParamBox.h"
#"ui/ParamBox.cpp"
"ui/Knob.cpp"
"ui/ChooseKnob.cpp"
"ui/UIMaker.h"
"ui/UIMaker.cpp"
"layout/SynthLayout.h"
"layout/SynthLayout.cpp" "param/timber_proc_param.h" "engine/timber_proc/hard_sync.h" "engine/timber_proc/hard_sync.cpp"  )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET AdditiveSynthEngine PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
target_link_libraries(AdditiveSynthEngine PUBLIC raylib)
target_link_libraries(AdditiveSynthEngine PUBLIC raygui_cpp)