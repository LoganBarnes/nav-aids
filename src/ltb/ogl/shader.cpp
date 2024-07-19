#include "ltb/ogl/shader.hpp"

// project
#include "ltb/ogl/object_id.hpp"

// external
#include <Shadinclude/Shadinclude.hpp>
#include <spdlog/fmt/fmt.h>

namespace ltb::ogl
{

template < GLenum shader_type >
    requires IsShaderType< shader_type >
auto Shader< shader_type >::initialize( ) -> utils::Result<>
{
    return create_shader( ).and_then( &Shader::load_and_compile ).map( utils::ToVoid{ } );
}

template < GLenum shader_type >
    requires IsShaderType< shader_type >
auto Shader< shader_type >::data( ) const -> ShaderData const&
{
    return data_;
}

template < GLenum shader_type >
    requires IsShaderType< shader_type >
auto Shader< shader_type >::create_shader( ) -> utils::Result< Shader* >
{
    auto const gl_id = glCreateShader( shader_type );
    if ( 0U == gl_id )
    {
        return LTB_MAKE_UNEXPECTED_ERROR(
            "Failed to create {} shader",
            to_string< shader_type >( )
        );
    }
    data_.gl_id = gl_id;
    spdlog::debug( "glCreateShader({})", data_.gl_id );

    deleter_ = make_deleter( data_.gl_id, glDeleteShader, "glDeleteShader" );
    return this;
}

template < GLenum shader_type >
    requires IsShaderType< shader_type >
auto Shader< shader_type >::load_and_compile( ) -> utils::Result< Shader* >
{
    if ( !std::filesystem::exists( filename_ ) )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "File not found: {}", filename_.string( ) );
    }

    // Load shader from disk
    auto const  shader_str    = Shadinclude::load( filename_.string( ) );
    char const* shader_source = shader_str.c_str( );

    if ( shader_str.empty( ) )
    {
        return LTB_MAKE_UNEXPECTED_ERROR(
            "Shader '{}' ({}): File is empty",
            filename_.filename( ).string( ),
            to_string< shader_type >( )
        );
    }

    auto const shader_id = data_.gl_id;

    // Compile shader
    glShaderSource( shader_id, 1, &shader_source, nullptr );
    glCompileShader( shader_id );

    // Check shader
    auto result = GLint{ GL_FALSE };
    glGetShaderiv( shader_id, GL_COMPILE_STATUS, &result );

    if ( GL_FALSE == result )
    {
        auto log_length = GLint{ 0 };
        glGetShaderiv( shader_id, GL_INFO_LOG_LENGTH, &log_length );

        if ( 0 == log_length )
        {
            return LTB_MAKE_UNEXPECTED_ERROR(
                "Shader '{}' ({}): Compilation failed for unknown reason",
                filename_.filename( ).string( ),
                to_string< shader_type >( )
            );
        }

        auto gl_error = std::vector< char >( static_cast< size_t >( log_length ) );
        glGetShaderInfoLog( shader_id, log_length, nullptr, gl_error.data( ) );

        return LTB_MAKE_UNEXPECTED_ERROR(
            "Shader '{}' ({}): {}",
            filename_.filename( ).string( ),
            to_string< shader_type >( ),
            std::string( gl_error.data( ) )
        );
    }

    return this;
}

#if defined( GL_COMPUTE_SHADER )
template class Shader< GL_COMPUTE_SHADER >;
#endif
template class Shader< GL_VERTEX_SHADER >;
template class Shader< GL_TESS_CONTROL_SHADER >;
template class Shader< GL_TESS_EVALUATION_SHADER >;
template class Shader< GL_GEOMETRY_SHADER >;
template class Shader< GL_FRAGMENT_SHADER >;

} // namespace ltb::ogl
