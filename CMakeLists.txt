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

option(
  LTB_NAV_BUILD_TESTS
  "Build unit test executables"
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
  ${CMAKE_CURRENT_LIST_DIR}/src/ltb/ltb_nav_config.hpp.in
  ${LTB_NAV_GENERATED_DIR}/ltb/nav/ltb_nav_config.hpp
)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/res/shaders)

# ##############################################################################
# Library
# ##############################################################################
file(
  GLOB_RECURSE
  LtbNav_TEST_FILES
  LIST_DIRECTORIES
  false
  CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_LIST_DIR}/src/ltb/*_tests.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/ltb/test/*.cpp
)
#file(
#  GLOB_RECURSE
#  LtbNav_SOURCE_FILES
#  LIST_DIRECTORIES
#  false
#  CONFIGURE_DEPENDS
#  ${CMAKE_CURRENT_LIST_DIR}/src/ltb/*.cpp
#)
#list(
#  REMOVE_ITEM
#  LtbNav_SOURCE_FILES
#  ${LtbNav_TEST_FILES}
#)

add_library(
  LtbNav
  #  ${LtbNav_SOURCE_FILES}
  ${CMAKE_CURRENT_LIST_DIR}/src/ltb/apps/ils/app.cpp
)
add_library(
  LtbNav::LtbNav
  ALIAS
  LtbNav
)
target_link_libraries(
  LtbNav
  PUBLIC
  # Shaders
  LtbNav::generate_spirv
  # Graphics
  LtbVlk::LtbVlk
)
target_include_directories(
  LtbNav
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  $<BUILD_INTERFACE:${LTB_NAV_GENERATED_DIR}>
)
target_include_directories(
  LtbNav
  SYSTEM
  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/external/src>
)
target_compile_features(
  LtbNav
  PUBLIC
  cxx_std_23
)
target_compile_definitions(
  LtbNav
  PUBLIC
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
if (${LTB_NAV_BUILD_TESTS})
  add_executable(
    TestLtbNav
    ${LtbNav_TEST_FILES}
  )
  target_link_libraries(
    TestLtbNav
    PRIVATE
    LtbNav::LtbNav
    GTest::gmock
    GTest::gtest_main
  )
  add_test(
    NAME
    TestLtbNav
    COMMAND
    $<TARGET_FILE:TestLtbNav>
  )
endif ()

# ##############################################################################
# Executables
# ##############################################################################

# Instrument Landing System (ILS)
add_executable(
  LtbIlsApp
  ${CMAKE_CURRENT_LIST_DIR}/src/ltb/apps/ils/main.cpp
)
target_link_libraries(
  LtbIlsApp
  PRIVATE
  LtbNav::LtbNav
)
target_include_directories(
  LtbIlsApp
  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>
)
