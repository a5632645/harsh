cmake_minimum_required (VERSION 3.8)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

add_subdirectory ("raylib-cpp")
add_subdirectory ("raylib")
add_subdirectory("raygui/projects/CMake")
add_subdirectory ("raygui-cpp")
add_subdirectory ("AudioFFT")
add_subdirectory ("AudioFile")
add_subdirectory ("gcem")
add_subdirectory("JUCE")

set(DOWNLOAD_DOCTEST ON)
add_subdirectory("xsimd")

include(FetchContent)

# json library
FetchContent_Declare(json 
URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_MakeAvailable(json)

# rtmidi
set_property(CACHE RTMIDI_TARGETNAME_UNINSTALL PROPERTY VALUE "my_uninstall")
FetchContent_Declare(rtmidi
URL http://www.music.mcgill.ca/~gary/rtmidi/release/rtmidi-6.0.0.tar.gz
DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_MakeAvailable(rtmidi)

set(USE_XSIMD OFF)
add_subdirectory ("AdditiveSynthEngine")
