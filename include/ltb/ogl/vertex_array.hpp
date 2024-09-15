#pragma once

// project
#include "ltb/ogl/buffer.hpp"
#include "ltb/ogl/fwd.hpp"
#include "ltb/ogl/opengl.hpp"
#include "ltb/ogl/utils.hpp"

// standard
#include <memory>
#include <optional>
#include <vector>

namespace ltb::ogl
{

struct VertexArrayData
{
    /// \brief OpenGL ID from `glGenVertexArrays()`
    GLuint gl_id = 0U;
};

class VertexArray
{
public:
    VertexArray( ) = default;

    /// \brief Initialize the vertex array object. This must
    ///        be called before using the vertex array.
    auto initialize( ) -> utils::Result<>;

    /// \brief Check if the vertex array has been successfully initialized.
    [[nodiscard( "Const getter" )]]
    auto is_initialized( ) const -> bool;

    /// \brief The raw settings stored for this vertex array
    [[nodiscard( "Const getter" )]]
    auto data( ) const -> VertexArrayData const&;

    static auto static_bind( VertexArray const& vertex_array ) -> void;
    static auto static_bind( GLuint raw_gl_id ) -> void;

private:
    VertexArrayData         data_    = { };
    std::shared_ptr< void > deleter_ = nullptr;
};

struct VaoElement
{
    GLuint      attribute_location;
    GLint       num_coordinates;
    GLenum      data_type;
    void const* initial_offset_into_vbo;
};

/// \brief Sets the vertex attribute information for the given vertex buffer object.
/// \param vbo the vertex buffer to which these attributes apply.
/// \param elements the individual attribute elements.
/// \param total_stride the byte offset between consecutive attributes (0 implies tightly packed).
/// \param attrib_divisor the number of instances between updates (0 if not using instancing).
auto set_attributes(
    Bound< VertexArray > const&             bound_vertex_array,
    Bound< Buffer, GL_ARRAY_BUFFER > const& bound_buffer,
    std::vector< VaoElement > const&        elements,
    GLsizei                                 total_stride,
    GLuint                                  attrib_divisor
) -> void;

} // namespace ltb::ogl
