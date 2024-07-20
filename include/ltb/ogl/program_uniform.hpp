#pragma once

// project
#include "ltb/ogl/buffer.hpp"
#include "ltb/ogl/fwd.hpp"
#include "ltb/ogl/program.hpp"
#include "ltb/ogl/texture.hpp"

// external
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

namespace ltb::ogl
{

template < typename ValueType >
class Uniform
{
public:
    using LocationType = std::conditional_t< std::is_same_v< ValueType, Buffer >, GLuint, GLint >;

    Uniform( Program& program, std::string name );

    auto initialize( ) -> utils::Result<>;

    [[nodiscard( "Const getter" )]]
    auto program_id( ) const -> GLuint;

    [[nodiscard( "Const getter" )]]
    auto location( ) const -> LocationType;

private:
    Program&     program_;
    std::string  name_;
    LocationType location_ = static_cast< LocationType >( GL_INVALID_INDEX );
};

template < typename ValueType >
Uniform< ValueType >::Uniform( Program& program, std::string name )
    : program_( program )
    , name_( std::move( name ) )
{
}

template < typename ValueType >
auto Uniform< ValueType >::initialize( ) -> utils::Result<>
{
    static_assert( static_cast< LocationType >( GL_INVALID_INDEX ) == -1 );

    location_ = glGetUniformLocation( program_.data( ).gl_id, name_.c_str( ) );
    if ( location_ < 0 )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "Uniform '{}' not found in program.", name_ );
    }
    spdlog::debug( "Uniform '{}' location: {}", name_, location_ );

    return utils::success( );
}

template < typename ValueType >
auto Uniform< ValueType >::program_id( ) const -> GLuint
{
    return program_.data( ).gl_id;
}

template < typename ValueType >
auto Uniform< ValueType >::location( ) const -> LocationType
{
    return location_;
}

template < typename ValueType >
concept IsUniformScalarType = IsAny< ValueType, int32, uint32, float32, float64 >;

template < typename UniformType, typename ValueType >
    requires IsUniformScalarType< ValueType >
auto set_scalar(
    Uniform< UniformType > const& uniform,
    glm::length_t const           size,
    ValueType const*              values,
    GLsizei const                 count
) -> void
{
    auto const index = static_cast< uint32 >( size - 1 );

    if constexpr ( std::is_same_v< ValueType, int32 > )
    {
        return std::array{
            glProgramUniform1iv,
            glProgramUniform2iv,
            glProgramUniform3iv,
            glProgramUniform4iv,
        }[ index ]( uniform.program_id( ), uniform.location( ), count, values );
    }
    else if constexpr ( std::is_same_v< ValueType, uint32 > )
    {
        return std::array{
            glProgramUniform1uiv,
            glProgramUniform2uiv,
            glProgramUniform3uiv,
            glProgramUniform4uiv,
        }[ index ]( uniform.program_id( ), uniform.location( ), count, values );
    }
    else if constexpr ( std::is_same_v< ValueType, float32 > )
    {
        return std::array{
            glProgramUniform1fv,
            glProgramUniform2fv,
            glProgramUniform3fv,
            glProgramUniform4fv,
        }[ index ]( uniform.program_id( ), uniform.location( ), count, values );
    }
    else if constexpr ( std::is_same_v< ValueType, float64 > )
    {
        return std::array{
            glProgramUniform1dv,
            glProgramUniform2dv,
            glProgramUniform3dv,
            glProgramUniform4dv,
        }[ index ]( uniform.program_id( ), uniform.location( ), count, values );
    }
    else
    {
        static_assert( 0 == sizeof( ValueType ), "Unhandled type" );
    }
}

template < typename ValueType >
    requires IsUniformScalarType< ValueType >
auto set( Uniform< ValueType > const& uniform, ValueType const& value ) -> void
{
    set_scalar( uniform, 1, &value, 1 );
}

template < glm::length_t Size, typename ValueType >
auto set(
    Uniform< glm::vec< Size, ValueType > > const& uniform,
    glm::vec< Size, ValueType > const&            value
) -> void
{
    set_scalar( uniform, Size, glm::value_ptr( value ), 1 );
}

template < typename UniformType, typename ValueType >
    requires IsUniformScalarType< ValueType >
auto set_matrix(
    Uniform< UniformType > const& uniform,
    glm::length_t const           size,
    GLboolean const               transpose,
    ValueType const*              values,
    GLsizei const                 count
) -> void
{
    auto const index = static_cast< uint32 >( size - 2 );

    if constexpr ( std::is_same_v< ValueType, float32 > )
    {
        return std::array{
            glProgramUniformMatrix2fv,
            glProgramUniformMatrix3fv,
            glProgramUniformMatrix4fv,
        }[ index ]( uniform.program_id( ), uniform.location( ), count, transpose, values );
    }
    else if constexpr ( std::is_same_v< ValueType, float64 > )
    {
        return std::array{
            glProgramUniformMatrix2dv,
            glProgramUniformMatrix3dv,
            glProgramUniformMatrix4dv,
        }[ index ]( uniform.program_id( ), uniform.location( ), count, transpose, values );
    }
    else
    {
        static_assert( 0 == sizeof( ValueType ), "Unhandled type" );
    }
}

template < glm::length_t Size, typename ValueType >
auto set(
    Uniform< glm::mat< Size, Size, ValueType > > const& uniform,
    glm::mat< Size, Size, ValueType > const&            value
) -> void
{
    set_matrix( uniform, Size, GL_FALSE, glm::value_ptr( value ), 1 );
}

auto set(
    Uniform< Buffer > const&                         uniform,
    Bound< Buffer, GL_SHADER_STORAGE_BUFFER > const& buffer,
    GLuint                                           binding,
    GLintptr                                         byte_offset,
    GLsizeiptr                                       size_in_bytes
) -> void;

template < GLenum bind_type >
auto set(
    Uniform< Texture > const&          uniform,
    Bound< Texture, bind_type > const& bound_texture,
    GLint const                        active_tex
) -> void
{
    // This argument is only provided to ensure the texture
    // is bound, but nothing has to be done with it.
    utils::ignore( bound_texture );

    glActiveTexture( static_cast< GLenum >( GL_TEXTURE0 + active_tex ) );
    glProgramUniform1i( uniform.program_id( ), uniform.location( ), active_tex );
}

} // namespace ltb::ogl
