#pragma once

// project
#include "ltb/ogl/buffer.hpp"
#include "ltb/ogl/program.hpp"
#include "ltb/ogl/texture.hpp"

// external
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

namespace ltb::ogl
{

/// \brief Type alias for boolean integer values.
/// \details Raw boolean values cannot be passed to OpenGL without a
///          conversion and this type can be used to make that explicit.
using BoolInt = int32;

template < typename ValueType >
class Uniform
{
public:
    using LocationType = std::conditional_t< std::is_same_v< ValueType, Buffer >, GLuint, GLint >;

    /// \brief Constructs a uniform with the given program and name.
    Uniform( Program& program, std::string name );

    /// \brief Initializes the uniform by querying its location in the program.
    auto initialize( ) -> utils::Result<>;

    /// \brief Returns whether the uniform has been successfully initialized.
    [[nodiscard( "Const getter" )]] auto is_initialized( ) const -> bool;

    /// \brief Returns the OpenGL ID of the parent program.
    [[nodiscard( "Const getter" )]] auto program_id( ) const -> GLuint;

    /// \brief Returns the location of the uniform in the program.
    [[nodiscard( "Const getter" )]] auto location( ) const -> LocationType;

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
    LTB_CHECK_VALID(
        program_.is_initialized( ),
        "Uniform program has not been successfully initialized."
    );

    static_assert( static_cast< LocationType >( GL_INVALID_INDEX ) == -1 );

    location_ = glGetUniformLocation( program_.data( ).gl_id, name_.c_str( ) );
    if ( location_ < 0 )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "Uniform '{}' not found in program.", name_ );
    }
    spdlog::debug( "Uniform '{}' location: {}", name_, location_ );

    return utils::success( );
}

template <>
auto Uniform< Buffer >::initialize( ) -> utils::Result<>;

template < typename ValueType >
auto Uniform< ValueType >::is_initialized( ) const -> bool
{
    return location_ != static_cast< LocationType >( GL_INVALID_INDEX );
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

namespace detail
{

// Only specific types are allowed for uniform values:
// https://registry.khronos.org/OpenGL-Refpages/es3.1/html/glProgramUniform.xhtml
template < typename ValueType >
concept IsUniformScalarType = IsAny< ValueType, int32, uint32, float32, float64 >;

// Only specific sizes are allowed for vec values:
// https://registry.khronos.org/OpenGL-Refpages/es3.1/html/glProgramUniform.xhtml
template < glm::length_t size >
concept IsScalarSize = ( 1 <= size && size <= 4 );

template < glm::length_t size, typename UniformType, typename ValueType >
    requires IsUniformScalarType< ValueType > && IsScalarSize< size >

auto set_scalar_or_vec(
    Uniform< UniformType > const& uniform,
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

// Only specific types are allowed for matrix uniform values:
// https://registry.khronos.org/OpenGL-Refpages/es3.1/html/glProgramUniform.xhtml
template < typename ValueType >
concept IsUniformMatrixType = IsAny< ValueType, float32, float64 >;

// Only specific sizes are allowed for mat values:
// https://registry.khronos.org/OpenGL-Refpages/es3.1/html/glProgramUniform.xhtml
template < glm::length_t size >
concept IsMatrixSize = ( 2 <= size && size <= 4 );

template < glm::length_t size, typename UniformType, typename ValueType >
    requires IsUniformMatrixType< ValueType > && IsMatrixSize< size >

auto set_matrix(
    Uniform< UniformType > const& uniform,
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

} // namespace detail

/// \brief Sets the value of a boolean uniform.
template < typename ValueType >
    requires detail::IsUniformScalarType< ValueType >

auto set( Uniform< ValueType > const& uniform, bool bvalue ) -> void
{
    auto const value = static_cast< ValueType >( bvalue );
    detail::set_scalar_or_vec< 1 >( uniform, &value, 1 );
}

/// \brief Sets the value of a scalar uniform.
template < typename ValueType >
    requires detail::IsUniformScalarType< ValueType >

auto set( Uniform< ValueType > const& uniform, ValueType const& value ) -> void
{
    detail::set_scalar_or_vec< 1 >( uniform, &value, 1 );
}

/// \brief Sets the value of a vector uniform.
template < glm::length_t size, typename ValueType >
    requires detail::IsUniformScalarType< ValueType >

auto set(
    Uniform< glm::vec< size, ValueType > > const& uniform,
    glm::vec< size, ValueType > const&            value
) -> void
{
    detail::set_scalar_or_vec< size >( uniform, glm::value_ptr( value ), 1 );
}

/// \brief Sets the value of a matrix uniform.
template < glm::length_t size, typename ValueType >
    requires detail::IsUniformMatrixType< ValueType >

auto set(
    Uniform< glm::mat< size, size, ValueType > > const& uniform,
    glm::mat< size, size, ValueType > const&            value
) -> void
{
    detail::set_matrix< size >( uniform, GL_FALSE, glm::value_ptr( value ), 1 );
}

/// \brief Sets the value of a buffer uniform.
auto set(
    Uniform< Buffer > const&                         uniform,
    Bound< Buffer, GL_SHADER_STORAGE_BUFFER > const& buffer,
    GLuint                                           binding
) -> void;

/// \brief Sets the value of a texture uniform.
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

/// \brief Sets the value of a Shader Storage Buffer Object (SSBO) uniform.
auto set(
    Uniform< Buffer > const&                         uniform,
    Bound< Buffer, GL_SHADER_STORAGE_BUFFER > const& buffer,
    GLuint const                                     binding,
    GLintptr const                                   byte_offset,
    GLsizeiptr const                                 size_in_bytes
) -> void;

} // namespace ltb::ogl
