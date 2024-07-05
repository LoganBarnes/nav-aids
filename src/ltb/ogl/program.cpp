#include "ltb/ogl/program.hpp"

// project
#include "ltb/ogl/buffer.hpp"
#include "ltb/ogl/object_id.hpp"
#include "ltb/utils/size_utils.hpp"
#include "ltb/utils/type_string.hpp"

// external
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

namespace ltb::ogl
{
namespace
{

constexpr auto scalar_uniform_size_range = math::Range< int32 >{
    .min = 1,
    .max = 4,
};
constexpr auto matrix_uniform_size_range = math::Range< int32 >{
    .min = 2,
    .max = 4,
};

template < typename ValueType >
auto* scalar_uniform_function( int32 const size )
{
    auto const index = static_cast< uint32 >( size - 1 );

    if constexpr ( std::is_same_v< ValueType, int32 > )
    {
        return std::array{
            glProgramUniform1iv,
            glProgramUniform2iv,
            glProgramUniform3iv,
            glProgramUniform4iv,
        }[ index ];
    }
    else if constexpr ( std::is_same_v< ValueType, uint32 > )
    {
        return std::array{
            glProgramUniform1uiv,
            glProgramUniform2uiv,
            glProgramUniform3uiv,
            glProgramUniform4uiv,
        }[ index ];
    }
    else if constexpr ( std::is_same_v< ValueType, float32 > )
    {
        return std::array{
            glProgramUniform1fv,
            glProgramUniform2fv,
            glProgramUniform3fv,
            glProgramUniform4fv,
        }[ index ];
    }
    else if constexpr ( std::is_same_v< ValueType, float64 > )
    {
        return std::array{
            glProgramUniform1dv,
            glProgramUniform2dv,
            glProgramUniform3dv,
            glProgramUniform4dv,
        }[ index ];
    }
    else
    {
        static_assert( 0 == sizeof( ValueType ), "Unhandled type" );
    }
}

template < typename ValueType >
auto* matrix_uniform_function( int32 const size )
{
    auto const index = static_cast< uint32 >( size - 2 );

    if constexpr ( std::is_same_v< ValueType, float32 > )
    {
        return std::array{
            glProgramUniformMatrix2fv,
            glProgramUniformMatrix3fv,
            glProgramUniformMatrix4fv,
        }[ index ];
    }
    else if constexpr ( std::is_same_v< ValueType, float64 > )
    {
        return std::array{
            glProgramUniformMatrix2dv,
            glProgramUniformMatrix3dv,
            glProgramUniformMatrix4dv,
        }[ index ];
    }
    else
    {
        static_assert( 0 == sizeof( ValueType ), "Unhandled type" );
    }
}

template < typename IndexType >
    requires is_index_type_v< IndexType >
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

auto Program::get_attribute_location( std::string const& name ) const -> utils::Result< GLuint >
{
    auto const loc = glGetAttribLocation( data( ).gl_id, name.c_str( ) );
    if ( loc < 0 )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "Cannot find attrib location for '{}'", name );
    }
    return static_cast< GLuint >( loc );
}

template < typename ValueType >
auto Program::set_scalar_or_vec_uniform(
    std::string const&     name,
    ValueType const* const value,
    int32 const            size,
    int32 const            count
) const -> utils::Result< Program const* >
{
    auto const location = glGetUniformLocation( data( ).gl_id, name.c_str( ) );
    if ( location < 0 )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "Uniform '{}' not found in program.", name );
    }

    if ( !contains( scalar_uniform_size_range, size ) )
    {
        return LTB_MAKE_UNEXPECTED_ERROR(
            "{} uniform of size {} is not supported.",
            utils::type_string< ValueType >( ),
            size
        );
    }

    scalar_uniform_function< ValueType >( size )( data( ).gl_id, location, count, value );

    return this;
}

template auto Program::set_scalar_or_vec_uniform( std::string const&, int32 const*, int32, int32 )
    const -> utils::Result< Program const* >;

template auto Program::set_scalar_or_vec_uniform( std::string const&, uint32 const*, int32, int32 )
    const -> utils::Result< Program const* >;

template auto Program::set_scalar_or_vec_uniform( std::string const&, float32 const*, int32, int32 )
    const -> utils::Result< Program const* >;

template auto Program::set_scalar_or_vec_uniform( std::string const&, float64 const*, int32, int32 )
    const -> utils::Result< Program const* >;

auto Program::set_scalar_or_vec_uniform(
    std::string const& name,
    bool const* const  value,
    int32 const        size,
    int32 const        count
) const -> utils::Result< Program const* >
{
    // The type used to represent boolean uniforms.
    // `int`, `uint`, or `float` types can be used.
    // `0` or `0.f` is `false`, everything else is `true`.
    using Bool = int32;

    // Convert boolean array to Bool array.
    auto const bool_count      = utils::total_size( size, count );
    auto       converted_value = std::vector< Bool >( bool_count );

    for ( auto i = 0UZ; i < bool_count; ++i )
    {
        converted_value[ i ] = static_cast< Bool >( value[ i ] );
    }

    return set_scalar_or_vec_uniform< Bool >( name, converted_value.data( ), size, count );
}

template < typename ValueType >
auto Program::set_matrix_uniform(
    std::string const&     name,
    ValueType const* const value,
    int32 const            size,
    int32 const            count
) const -> utils::Result< Program const* >
{
    auto const location = glGetUniformLocation( data( ).gl_id, name.c_str( ) );
    if ( location < 0 )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "Uniform '{}' not found in program.", name );
    }

    if ( !contains( matrix_uniform_size_range, size ) )
    {
        return LTB_MAKE_UNEXPECTED_ERROR(
            "{} matrix uniform of size {} is not supported.",
            utils::type_string< ValueType >( ),
            size
        );
    }

    matrix_uniform_function< ValueType >( size )( data( ).gl_id, location, count, GL_FALSE, value );

    return this;
}

template auto Program::set_matrix_uniform( std::string const&, float const*, int32, int32 ) const
    -> utils::Result< Program const* >;

template auto Program::set_matrix_uniform( std::string const&, double const*, int32, int32 ) const
    -> utils::Result< Program const* >;

auto Program::set_uniform(
    std::string const&                               name,
    Bound< Buffer, GL_SHADER_STORAGE_BUFFER > const& buffer,
    GLuint const                                     binding,
    GLintptr const                                   byte_offset,
    GLsizeiptr const                                 size_in_bytes
) const -> utils::Result< Program const* >
{
    auto const block_index
        = glGetProgramResourceIndex( data( ).gl_id, GL_SHADER_STORAGE_BLOCK, name.c_str( ) );

    if ( GL_INVALID_INDEX == block_index )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "Uniform '{}' not found in program.", name );
    }
    glShaderStorageBlockBinding( data( ).gl_id, block_index, binding );

    ogl::bind_buffer_range( buffer, binding, byte_offset, size_in_bytes );
    return this;
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
    requires is_index_type_v< IndexType >
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
    requires is_index_type_v< IndexType >
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
