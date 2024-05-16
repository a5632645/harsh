cmake_minimum_required (VERSION 3.8)
project ("AdditiveSynthEngine")

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


if(WIN32)
    add_compile_options(/wd4819)
    add_compile_options("/arch:AVX")
endif()

# sub modules
add_subdirectory ("raylib-cpp")
add_subdirectory ("raylib")

add_subdirectory("raygui/projects/CMake")

add_subdirectory ("raygui-cpp")
add_subdirectory ("AudioFFT")
add_subdirectory ("AudioFile")
add_subdirectory ("gcem")

set(DOWNLOAD_DOCTEST ON)
add_subdirectory("xsimd")

include(FetchContent)

# json library
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

# rtmidi
FetchContent_Declare(rtmidi URL http://www.music.mcgill.ca/~gary/rtmidi/release/rtmidi-6.0.0.tar.gz)
FetchContent_MakeAvailable(rtmidi)

add_subdirectory ("AdditiveSynthEngine")
