# ##############################################################################
# A Logan Thomas Barnes project
# ##############################################################################
cmake_minimum_required(VERSION 3.23)
project(LtbNav VERSION 0.0.1)

# Prevent cyclic dependencies even for static libs
set(GLOBAL_DEPENDS_NO_CYCLES ON)

# Prevent cmake from creating build artifacts in the source directory
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# ##############################################################################
# Options
# ##############################################################################
option(
  LTB_NAV_USE_STRICT_FLAGS
  "Use strict flags when building"
  OFF
)

# ##############################################################################
# CMake Package Manager
# ##############################################################################
include(cmake/CPM.cmake)

# ##############################################################################
# External Packages
# ##############################################################################
include(${CMAKE_CURRENT_LIST_DIR}/cmake/ThirdParty.cmake)

# ##############################################################################
# Configuration
# ##############################################################################
# A directory where generated files can be stored and referenced
set(LTB_NAV_GENERATED_DIR ${CMAKE_BINARY_DIR}/generated)

configure_file(
  ${CMAKE_CURRENT_LIST_DIR}/src/ltb/ltb_config.hpp.in
  ${LTB_NAV_GENERATED_DIR}/ltb/ltb_config.hpp
)

if (TARGET Vulkan::Vulkan)
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/res/shaders)
endif ()

# ##############################################################################
# Library
# ##############################################################################
file(
  GLOB_RECURSE
  LtbNav_SOURCE_FILES
  LIST_DIRECTORIES
  false
  CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_LIST_DIR}/external/src/*.c
  ${CMAKE_CURRENT_LIST_DIR}/src/ltb/*.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/ltb/*.hpp
)

add_library(
  LtbNav
  ${LtbNav_SOURCE_FILES}
)
add_library(
  LtbNav::LtbNav
  ALIAS
  LtbNav
)
target_link_libraries(
  LtbNav
  PUBLIC
  # C++ Utils
  Threads::Threads
  tl::expected
  magic_enum::magic_enum
  range-v3::range-v3
  spdlog::spdlog
  $<TARGET_NAME_IF_EXISTS:TBB::tbb>
  # Geometry
  glm::glm
  # Gui
  imgui::imgui
)
target_include_directories(
  LtbNav
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>
  $<BUILD_INTERFACE:${LTB_NAV_GENERATED_DIR}>
)
target_include_directories(
  LtbNav
  SYSTEM
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/external/include>
  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/external/src>
)
target_compile_features(
  LtbNav
  PUBLIC
  cxx_std_20
)
target_compile_definitions(
  LtbNav
  PUBLIC
  $<$<COMPILE_LANGUAGE:CXX>:VULKAN_HPP_NO_EXCEPTIONS>
  # Deal with windows warnings and macros.
  $<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_WARNINGS>
  $<$<PLATFORM_ID:Windows>:NOMINMAX>
)
set_target_properties(
  LtbNav
  PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  WINDOWS_EXPORT_ALL_SYMBOLS ON
)
target_compile_options(
  LtbNav
  PUBLIC
  # Ignore any headers using angle brackets on windows
  $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/experimental:external>
  $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/external:anglebrackets>
  $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/external:W0>
  # Hopefully temporary MSVC spdlog warning
  $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/wd4996>
)

# ##############################################################################
# Development Settings
# ##############################################################################
if (LTB_NAV_USE_STRICT_FLAGS)
  target_compile_options(
    LtbNav
    PUBLIC
    # Strict warnings/errors with gcc and clang
    $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wall>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wextra>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Werror>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wpedantic>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wunused>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-pedantic-errors>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Winit-self>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wold-style-cast>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Woverloaded-virtual>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wsign-conversion>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wshadow>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wmissing-declarations>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wmissing-include-dirs>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wno-unknown-pragmas>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wformat-truncation>
    $<$<COMPILE_LANG_AND_ID:CXX,Clang,AppleClang>:-Wimplicit-float-conversion>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wfloat-conversion>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-fstrict-aliasing>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wstrict-aliasing>
    $<$<COMPILE_LANG_AND_ID:CXX,Clang,AppleClang>:-Wcast-align>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wcast-align=strict>
    # Strict warnings/errors with msvc
    $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/W4>
    $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX>
  )
endif ()

# ##############################################################################
# Tests
# ##############################################################################

# ##############################################################################
# Executable
# ##############################################################################
add_executable(
  LtbNavApp
  ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
)
target_link_libraries(
  LtbNavApp
  PRIVATE
  LtbNav::LtbNav
)
