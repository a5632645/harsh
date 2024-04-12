# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)
project ("AdditiveSynthEngine")

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  add_compile_definitions(MANA_DEBUG)
else()
  
endif()

if(WIN32)
    add_compile_options(/wd4819)
endif()


# 包含子项目。
add_subdirectory ("AdditiveSynthEngine")
add_subdirectory ("raylib-cpp")
add_subdirectory ("raylib")
add_subdirectory ("raygui-cpp")