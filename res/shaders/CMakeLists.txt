# ##############################################################################
# A Logan Thomas Barnes project
# ##############################################################################
file(
  GLOB_RECURSE LtbNav_SHADERS
  LIST_DIRECTORIES false
  CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_LIST_DIR}/ils/*.vert
  ${CMAKE_CURRENT_LIST_DIR}/ils/*.geom
  ${CMAKE_CURRENT_LIST_DIR}/ils/*.tesc
  ${CMAKE_CURRENT_LIST_DIR}/ils/*.tese
  ${CMAKE_CURRENT_LIST_DIR}/ils/*.frag
  ${CMAKE_CURRENT_LIST_DIR}/ils/*.comp
)
file(
  GLOB_RECURSE LtbNav_SHADER_DEPS
  LIST_DIRECTORIES false
  CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_LIST_DIR}/*.glsl
)

add_library(
  ltb_nav_generate_spirv
  INTERFACE
)
add_library(
  LtbNav::generate_spirv
  ALIAS
  ltb_nav_generate_spirv
)

# Generate the SPIR-V file for each shader
foreach( SHADER_FILE ${LtbNav_SHADERS} )
  set( SPIRV_FILE ${SHADER_FILE}.spv )

  add_custom_command(
    OUTPUT ${SPIRV_FILE}
    COMMAND $<TARGET_FILE:Vulkan::glslc>
    ARGS ${SHADER_FILE} -o ${SPIRV_FILE}
    DEPENDS ${SHADER_FILE} ${LtbNav_SHADER_DEPS}
    VERBATIM
  )

  set_source_files_properties(
    ${SPIRV_FILE}
    PROPERTIES
    GENERATED TRUE
  )
  target_sources(
    ltb_nav_generate_spirv
    PRIVATE
    ${SPIRV_FILE}
  )
endforeach()
