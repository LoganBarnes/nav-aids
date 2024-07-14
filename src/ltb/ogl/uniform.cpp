#include "ltb/ogl/uniform.hpp"

namespace ltb::ogl
{

auto set(
    Uniform< Buffer > const&                         uniform,
    Bound< Buffer, GL_SHADER_STORAGE_BUFFER > const& buffer,
    GLuint                                           binding,
    GLintptr                                         byte_offset,
    GLsizeiptr                                       size_in_bytes
) -> void
{
    glShaderStorageBlockBinding( uniform.program_id( ), uniform.block_index( ), binding );

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
