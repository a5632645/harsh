add_executable(AdSynth_Test
param_test.cpp)

set_property(TARGET AdSynth_Test PROPERTY CXX_STANDARD 20)

target_link_libraries(AdSynth_Test PRIVATE AdSynthLib)