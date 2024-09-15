#include "ltb/ogl/vertex_array.hpp"

// project
#include "ltb/ogl/object_id.hpp"

namespace ltb::ogl
{

auto VertexArray::initialize( ) -> utils::Result<>
{
    glGenVertexArrays( 1, &data_.gl_id );

    spdlog::debug( "glGenVertexArrays({})", data_.gl_id );
    deleter_ = make_array_deleter( { data_.gl_id }, glDeleteVertexArrays, "glDeleteVertexArrays" );

    return utils::success( );
}

auto VertexArray::is_initialized( ) const -> bool
{
    return 0U != data( ).gl_id;
}

auto VertexArray::data( ) const -> VertexArrayData const&
{
    return data_;
}

auto VertexArray::static_bind( VertexArray const& vertex_array ) -> void
{
    VertexArray::static_bind( vertex_array.data( ).gl_id );
}

auto VertexArray::static_bind( GLuint const raw_gl_id ) -> void
{
    glBindVertexArray( raw_gl_id );
}

auto set_attributes(
    Bound< VertexArray > const&             bound_vertex_array,
    Bound< Buffer, GL_ARRAY_BUFFER > const& bound_buffer,
    std::vector< VaoElement > const&        elements,
    GLsizei const                           total_stride,
    GLuint const                            attrib_divisor
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
