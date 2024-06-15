add_executable(peak "peak.cpp")

target_link_libraries(peak PRIVATE raylib)
target_link_libraries(peak PRIVATE gcem)
target_link_libraries(peak PRIVATE AudioFFT)
target_link_libraries(peak PRIVATE AudioFile)
set_property(TARGET peak PROPERTY CXX_STANDARD 20)