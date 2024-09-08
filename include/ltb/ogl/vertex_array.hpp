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

template < typename DataType >
struct VaoElement
{
    GLuint          attribute_location;
    GLint           num_coordinates;
    GLenum          data_type;
    DataType const* initial_offset_into_vbo;
};

/// \brief Sets the vertex attribute information for the given vertex buffer object.
/// \param vbo the vertex buffer to which these attributes apply.
/// \param elements the individual attribute elements.
/// \param total_stride the byte offset between consecutive attributes (0 implies tightly packed).
/// \param attrib_divisor the number of instances between updates (0 if not using instancing).
template < typename DataType >
auto set_attributes(
    Bound< VertexArray > const&                  bound_vertex_array,
    Bound< Buffer, GL_ARRAY_BUFFER > const&      bound_buffer,
    std::vector< VaoElement< DataType > > const& elements,
    GLsizei const                                total_stride,
    GLuint const                                 attrib_divisor
) -> void
{
    // These arguments are provided to ensure they are
    // bound, but nothing has to be done with them.
    utils::ignore( bound_vertex_array, bound_buffer );

    for ( auto const& element : elements )
    {
        glEnableVertexAttribArray( element.attribute_location );

        switch ( element.data_type )
        {
            case GL_BYTE:
            case GL_UNSIGNED_BYTE:
            case GL_SHORT:
            case GL_UNSIGNED_SHORT:
            case GL_INT:
            case GL_UNSIGNED_INT:
                glVertexAttribIPointer(
                    element.attribute_location,
                    element.num_coordinates,
                    element.data_type,
                    total_stride,
                    element.initial_offset_into_vbo
                );
                break;

            case GL_DOUBLE:
                glVertexAttribLPointer(
                    element.attribute_location,
                    element.num_coordinates,
                    element.data_type,
                    total_stride,
                    element.initial_offset_into_vbo
                );
                break;

            default:
                glVertexAttribPointer(
                    element.attribute_location,
                    element.num_coordinates,
                    element.data_type,
                    GL_FALSE,
                    total_stride,
                    element.initial_offset_into_vbo
                );
                break;
        }

        glVertexAttribDivisor( element.attribute_location, attrib_divisor );
    }
}

} // namespace ltb::ogl
