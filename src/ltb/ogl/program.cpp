#include "ltb/ogl/program.hpp"

// project
#include "ltb/ogl/object_id.hpp"
#include "ltb/ogl/vertex_array.hpp"

// external
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

namespace ltb::ogl
{
namespace
{

template < typename IndexType >
    requires IsIndexType< IndexType >
[[nodiscard( "Compile-time constant" )]]
constexpr auto index_type( ) -> GLenum
{
    if constexpr ( std::is_same_v< IndexType, GLubyte > )
    {
        return GL_UNSIGNED_BYTE;
    }
    else if constexpr ( std::is_same_v< IndexType, GLushort > )
    {
        return GL_UNSIGNED_SHORT;
    }
    else if constexpr ( std::is_same_v< IndexType, GLuint > )
    {
        return GL_UNSIGNED_INT;
    }
    else
    {
        static_assert( 0 == sizeof( IndexType ), "Unhandled type" );
    }
}

} // namespace

auto Program::initialize( ) -> utils::Result<>
{
    data_.gl_id = glCreateProgram( );
    if ( 0U == data_.gl_id )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "Failed to create program" );
    }

    spdlog::debug( "glCreateProgram({})", data_.gl_id );
    deleter_ = ogl::make_deleter( data_.gl_id, glDeleteProgram, "glDeleteProgram" );

    return utils::success( );
}

auto Program::is_initialized( ) const -> bool
{
    return 0U != data_.gl_id;
}

auto Program::data( ) const -> ProgramData const&
{
    return data_;
}

auto Program::static_bind( Program const& program ) -> void
{
    return Program::static_bind( program.data( ).gl_id );
}

auto Program::static_bind( GLuint const raw_gl_id ) -> void
{
    glUseProgram( raw_gl_id );
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

template < typename IndexType >
    requires IsIndexType< IndexType >
auto draw(
    Bound< Program > const&                         bound_program,
    Bound< VertexArray > const&                     bound_vertex_array,
    Bound< Buffer, GL_ELEMENT_ARRAY_BUFFER > const& bound_index_buffer,
    GLenum const                                    mode,
    IndexType const*                                index_offset,
    GLsizei const                                   num_elements
) -> void
{
    utils::ignore( bound_program, bound_vertex_array, bound_index_buffer );

    if ( num_elements > 0 )
    {
        glDrawElements( mode, num_elements, index_type< IndexType >( ), index_offset );
    }
}

template auto draw(
    Bound< Program > const&,
    Bound< VertexArray > const&,
    Bound< Buffer, GL_ELEMENT_ARRAY_BUFFER > const&,
    GLenum,
    GLubyte const*,
    GLsizei
) -> void;

template auto draw(
    Bound< Program > const&,
    Bound< VertexArray > const&,
    Bound< Buffer, GL_ELEMENT_ARRAY_BUFFER > const&,
    GLenum,
    GLushort const*,
    GLsizei
) -> void;

template auto draw(
    Bound< Program > const&,
    Bound< VertexArray > const&,
    Bound< Buffer, GL_ELEMENT_ARRAY_BUFFER > const&,
    GLenum,
    GLuint const*,
    GLsizei
) -> void;

auto draw_instanced(
    Bound< Program > const&     bound_program,
    Bound< VertexArray > const& bound_vertex_array,
    GLenum const                mode,
    GLsizei const               start,
    GLsizei const               num_elements,
    GLsizei const               instance_count
) -> void
{
    utils::ignore( bound_program, bound_vertex_array );

    if ( num_elements > 0 )
    {
        glDrawArraysInstanced( mode, start, num_elements, instance_count );
    }
}

template < typename IndexType >
    requires IsIndexType< IndexType >
auto draw_instanced(
    Bound< Program > const&                         bound_program,
    Bound< VertexArray > const&                     bound_vertex_array,
    Bound< Buffer, GL_ELEMENT_ARRAY_BUFFER > const& bound_index_buffer,
    GLenum const                                    mode,
    IndexType const*                                index_offset,
    GLsizei const                                   num_elements,
    GLsizei const                                   instance_count
) -> void
{
    utils::ignore( bound_program, bound_vertex_array, bound_index_buffer );

    if ( num_elements > 0 )
    {
        glDrawElementsInstanced(
            mode,
            num_elements,
            index_type< IndexType >( ),
            index_offset,
            instance_count
        );
    }
}

template auto draw_instanced(
    Bound< Program > const&,
    Bound< VertexArray > const&,
    Bound< Buffer, GL_ELEMENT_ARRAY_BUFFER > const&,
    GLenum,
    GLubyte const*,
    GLsizei,
    GLsizei
) -> void;

template auto draw_instanced(
    Bound< Program > const&,
    Bound< VertexArray > const&,
    Bound< Buffer, GL_ELEMENT_ARRAY_BUFFER > const&,
    GLenum,
    GLushort const*,
    GLsizei,
    GLsizei
) -> void;

template auto draw_instanced(
    Bound< Program > const&,
    Bound< VertexArray > const&,
    Bound< Buffer, GL_ELEMENT_ARRAY_BUFFER > const&,
    GLenum,
    GLuint const*,
    GLsizei,
    GLsizei
) -> void;

} // namespace ltb::ogl
