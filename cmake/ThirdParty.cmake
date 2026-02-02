# ##############################################################################
# A Logan Thomas Barnes project
# ##############################################################################

# Graphics APIs
find_package( Vulkan REQUIRED )

cpmaddpackage(
  NAME
  LtbVlk
  GITHUB_REPOSITORY
  LoganBarnes/ltb-vlk
  GIT_TAG
  9ee20b5
)

#cpmaddpackage(
#  NAME
#  implot
#  GITHUB_REPOSITORY
#  epezent/implot
#  GIT_TAG
#  f156599 # master: Jan 22, 2024
#  DOWNLOAD_ONLY
#  TRUE
#)
#cpmaddpackage("gh:apolukhin/pfr_non_boost#2.2.0")

if (${LTB_NAV_BUILD_TESTS})
  cpmaddpackage(
    NAME
    googletest
    GITHUB_REPOSITORY
    google/googletest
    GIT_TAG
    v1.15.2
    OPTIONS
    "INSTALL_GTEST OFF"
    "gtest_force_shared_crt ON"
  )

  # Enable testing
  include(CTest)
endif ()

#if (implot_ADDED)
#  add_library(
#    implot
#    ${implot_SOURCE_DIR}/implot.cpp
#    ${implot_SOURCE_DIR}/implot.h
#    ${implot_SOURCE_DIR}/implot_demo.cpp
#    ${implot_SOURCE_DIR}/implot_internal.h
#    ${implot_SOURCE_DIR}/implot_items.cpp
#  )
#  add_library(
#    implot::implot
#    ALIAS
#    implot
#  )
#  target_link_libraries(
#    implot
#    PUBLIC
#    imgui::imgui
#  )
#  target_include_directories(
#    implot
#    SYSTEM
#    PUBLIC
#    # Mark external include directories as system includes to avoid warnings.
#    $<BUILD_INTERFACE:${implot_SOURCE_DIR}>
#  )
#  target_compile_options(
#    implot
#    PRIVATE
#    # Ignore build warnings on this third-party code
#    $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-w>
#  )
#endif ()
