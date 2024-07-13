#pragma once

// project
#include "ltb/ogl/buffer.hpp"
#include "ltb/ogl/fwd.hpp"
#include "ltb/ogl/program.hpp"
#include "ltb/ogl/texture.hpp"

// external
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ltb::ogl
{

template < typename ValueType >
constexpr auto is_uniform_basic_type_v = is_any_same_v<
    ValueType,
    int32,
    uint32,
    float32,
    float64,
    glm::ivec2,
    glm::uvec2,
    glm::vec2,
    glm::dvec2,
    glm::ivec3,
    glm::uvec3,
    glm::vec3,
    glm::dvec3,
    glm::ivec4,
    glm::uvec4,
    glm::vec4,
    glm::dvec4,
    glm::mat2,
    glm::mat3,
    glm::mat4,
    glm::dmat2,
    glm::dmat3,
    glm::dmat4 >;

template < typename ValueType >
constexpr auto is_uniform_scalar_type_v
    = is_any_same_v< ValueType, int32, uint32, float32, float64 >;

template < typename ValueType, GLenum bind_type >
class Uniform
{
public:
    explicit Uniform( Program& program )
        : program_( program )
    {
    }

    auto initialize( std::string const& name ) -> utils::Result<>
    {
        location_ = glGetUniformLocation( program_.data( ).gl_id, name.c_str( ) );
        if ( location_ < 0 )
        {
            return LTB_MAKE_UNEXPECTED_ERROR( "Uniform '{}' not found in program.", name );
        }

        return utils::success( );
    }

    auto program_id( ) const -> GLuint { return program_.data( ).gl_id; }

    auto location( ) const -> GLint { return location_; }

private:
    Program& program_;
    GLint    location_ = -1;
};

template <>
class Uniform< Buffer, GL_SHADER_STORAGE_BUFFER >
{
public:
    explicit Uniform( Program& program )
        : program_( program )
    {
    }

    auto initialize( std::string const& name ) -> utils::Result<>
    {
        block_index_
            = glGetProgramResourceIndex( program_id( ), GL_SHADER_STORAGE_BLOCK, name.c_str( ) );

        if ( GL_INVALID_INDEX == block_index_ )
        {
            return LTB_MAKE_UNEXPECTED_ERROR( "Uniform '{}' not found in program.", name );
        }

        return utils::success( );
    }

    auto program_id( ) const -> GLuint { return program_.data( ).gl_id; }

    auto block_index( ) const -> GLuint { return block_index_; }

private:
    Program& program_;
    GLuint   block_index_ = GL_INVALID_INDEX;
};

template < typename UniformType, typename ValueType >
    requires is_uniform_scalar_type_v< ValueType >
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
    requires is_uniform_scalar_type_v< ValueType >
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
    requires is_uniform_scalar_type_v< ValueType >
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
    Uniform< Buffer, GL_SHADER_STORAGE_BUFFER > const& uniform,
    Bound< Buffer, GL_SHADER_STORAGE_BUFFER > const&   buffer,
    GLuint                                             binding,
    GLintptr                                           byte_offset,
    GLsizeiptr                                         size_in_bytes
) -> void;

template < GLenum bind_type >
auto set(
    Uniform< Texture, bind_type > const& uniform,
    Bound< Texture, bind_type > const&   bound_texture,
    GLint const                          active_tex
) -> void
{
    // This argument is only provided to ensure the texture
    // is bound, but nothing has to be done with it.
    utils::ignore( bound_texture );

    glActiveTexture( static_cast< GLenum >( GL_TEXTURE0 + active_tex ) );
    glProgramUniform1i( uniform.program_id( ), uniform.location( ), active_tex );
}

} // namespace ltb::ogl
