add_executable(AdSynth_Test
param_test.cpp)
add_executable(AdSynth_Resynthsis_Test
resynthsis_test.cpp)

set_property(TARGET AdSynth_Test PROPERTY CXX_STANDARD 20)
set_property(TARGET AdSynth_Resynthsis_Test PROPERTY CXX_STANDARD 20)

target_link_libraries(AdSynth_Test PRIVATE AdSynthLib)
target_link_libraries(AdSynth_Resynthsis_Test PRIVATE AdSynthLib)

add_executable(AdSynth_fft_Test
fft_test.cpp)
target_link_libraries(AdSynth_fft_Test PRIVATE AdSynthLib)
set_property(TARGET AdSynth_fft_Test PROPERTY CXX_STANDARD 20)
set_target_properties(AdSynth_fft_Test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin/debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/release"
)

set_target_properties(AdSynth_fft_Test PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)