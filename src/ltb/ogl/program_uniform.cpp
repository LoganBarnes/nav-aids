#include "ltb/ogl/program_uniform.hpp"

namespace ltb::ogl
{

template <>
auto Uniform< Buffer >::initialize( ) -> utils::Result<>
{
    location_ = glGetProgramResourceIndex( program_id( ), GL_SHADER_STORAGE_BLOCK, name_.c_str( ) );
    if ( GL_INVALID_INDEX == location_ )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "Uniform '{}' not found in program.", name_ );
    }
    spdlog::debug( "Buffer uniform '{}' location: {}", name_, location_ );

    return utils::success( );
}

auto set(
    Uniform< Buffer > const&                         uniform,
    Bound< Buffer, GL_SHADER_STORAGE_BUFFER > const& buffer,
    GLuint const                                     binding,
    GLintptr const                                   byte_offset,
    GLsizeiptr const                                 size_in_bytes
) -> void
{
    glShaderStorageBlockBinding( uniform.program_id( ), uniform.location( ), binding );

    ogl::bind_buffer_range( buffer, binding, byte_offset, size_in_bytes );
}

auto draw(
    Bound< Program > const&     bound_program,
    Bound< VertexArray > const& bound_vertex_array,
    GLenum const                mode,
    GLsizei const               start,
    GLsizei const               num_elements
) -> void
{
    utils::ignore( bound_program, bound_vertex_array );

    if ( num_elements > 0 )
    {
        glDrawArrays( mode, start, num_elements );
    }
}

} // namespace ltb::ogl
