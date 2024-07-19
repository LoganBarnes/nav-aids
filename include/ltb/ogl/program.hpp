#pragma once

// project
#include "ltb/ogl/shader.hpp"
#include "ltb/ogl/type_traits.hpp"
#include "ltb/ogl/utils.hpp"
#include "ltb/ogl/vertex_array.hpp"
#include "ltb/utils/types.hpp"

// standard
#include <filesystem>
#include <memory>
#include <optional>

namespace ltb::ogl
{

struct ProgramData
{
    /// \brief OpenGL ID from `glCreateProgram()`
    GLuint gl_id = 0U;
};

class Program
{
public:
    Program( ) = default;

    /// \brief Initialize the program object. This must
    ///        be called before using the program.
    auto initialize( ) -> utils::Result<>;

    /// \brief The raw settings stored for this program.
    [[nodiscard( "Const getter" )]]
    auto data( ) const -> ProgramData const&;

    /// \brief Attach and link the given shaders to the program.
    template < GLenum... ShaderTypes >
    auto attach_and_link( Shader< ShaderTypes > const&... shaders ) const -> utils::Result<>;

    static auto static_bind( Program const& program ) -> void;
    static auto static_bind( GLuint raw_gl_id ) -> void;

private:
    ProgramData             data_    = { };
    std::shared_ptr< void > deleter_ = nullptr;
};

template < GLenum... ShaderTypes >
auto Program::attach_and_link( Shader< ShaderTypes > const&... shaders ) const -> utils::Result<>
{
    auto const id = data( ).gl_id;

    // Attach all shaders to program.
    ( glAttachShader( id, shaders.data( ).gl_id ), ... );

    // Attempt to link program.
    glLinkProgram( id );

    // Shaders are no longer needed after linking.
    ( glDetachShader( id, shaders.data( ).gl_id ), ... );

    // Check program for linking errors.
    auto result = GLint{ GL_FALSE };
    glGetProgramiv( id, GL_LINK_STATUS, &result );

    if ( GL_FALSE == result )
    {
        auto log_length = GLint{ 0 };
        glGetProgramiv( id, GL_INFO_LOG_LENGTH, &log_length );

        if ( 0 == log_length )
        {
            return LTB_MAKE_UNEXPECTED_ERROR( "Program linking failed for unknown reason" );
        }

        auto gl_error = std::vector< char >( static_cast< size_t >( log_length ) );
        glGetProgramInfoLog( id, log_length, nullptr, gl_error.data( ) );

        return LTB_MAKE_UNEXPECTED_ERROR( "Program: {}", gl_error.data( ) );
    }

    return utils::success( );
}

template < typename IndexType >
concept IsIndexType = IsAny< IndexType, GLubyte, GLushort, GLuint >;

auto draw(
    Bound< Program > const&     bound_program,
    Bound< VertexArray > const& bound_vertex_array,
    GLenum                      mode,
    GLsizei                     start,
    GLsizei                     num_elements
) -> void;

template < typename IndexType >
    requires IsIndexType< IndexType >
auto draw(
    Bound< Program > const&                         bound_program,
    Bound< VertexArray > const&                     bound_vertex_array,
    Bound< Buffer, GL_ELEMENT_ARRAY_BUFFER > const& bound_index_buffer,
    GLenum                                          mode,
    IndexType const*                                index_offset,
    GLsizei                                         num_elements
) -> void;

auto draw_instanced(
    Bound< Program > const&     bound_program,
    Bound< VertexArray > const& bound_vertex_array,
    GLenum                      mode,
    GLsizei                     start,
    GLsizei                     num_elements,
    GLsizei                     instance_count
) -> void;

template < typename IndexType >
    requires IsIndexType< IndexType >
auto draw_instanced(
    Bound< Program > const&                         bound_program,
    Bound< VertexArray > const&                     bound_vertex_array,
    Bound< Buffer, GL_ELEMENT_ARRAY_BUFFER > const& bound_index_buffer,
    GLenum                                          mode,
    IndexType const*                                index_offset,
    GLsizei                                         num_elements,
    GLsizei                                         instance_count
) -> void;

} // namespace ltb::ogl
