#include "ltb/ogl/shader.hpp"

// project
#include "ltb/ogl/object_id.hpp"

// external
#include <Shadinclude/Shadinclude.hpp>
#include <spdlog/fmt/fmt.h>

namespace ltb::ogl
{

template < GLenum shader_type >
    requires is_shader_type_v< shader_type >
auto Shader< shader_type >::initialize( ) -> utils::Result<>
{
    data_.gl_id = glCreateShader( shader_type );
    if ( 0U == data_.gl_id )
    {
        return LTB_MAKE_UNEXPECTED_ERROR(
            "Failed to create {} shader",
            to_string< shader_type >( )
        );
    }

    spdlog::debug( "glCreateShader({})", data_.gl_id );
    deleter_ = make_deleter( data_.gl_id, glDeleteShader, "glDeleteShader" );
    return utils::success( );
}

template < GLenum shader_type >
    requires is_shader_type_v< shader_type >
auto Shader< shader_type >::data( ) const -> ShaderData const&
{
    return data_;
}

template < GLenum shader_type >
    requires is_shader_type_v< shader_type >
auto Shader< shader_type >::load_and_compile( std::filesystem::path const& filename
) const -> utils::Result<>
{
    if ( !std::filesystem::exists( filename ) )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "File not found: {}", filename.string( ) );
    }

    // Load shader from disk
    auto const  shader_str    = Shadinclude::load( filename.string( ) );
    char const* shader_source = shader_str.c_str( );

    if ( shader_str.empty( ) )
    {
        return LTB_MAKE_UNEXPECTED_ERROR(
            "Shader '{}' ({}): File is empty",
            filename.filename( ).string( ),
            to_string< shader_type >( )
        );
    }

    auto const id = data( ).gl_id;

    // Compile shader
    glShaderSource( id, 1, &shader_source, nullptr );
    glCompileShader( id );

    // Check shader
    auto result = GLint{ GL_FALSE };
    glGetShaderiv( id, GL_COMPILE_STATUS, &result );

    if ( GL_FALSE == result )
    {
        auto log_length = GLint{ 0 };
        glGetShaderiv( id, GL_INFO_LOG_LENGTH, &log_length );

        if ( 0 == log_length )
        {
            return LTB_MAKE_UNEXPECTED_ERROR(
                "Shader '{}' ({}): Compilation failed for unknown reason",
                filename.filename( ).string( ),
                to_string< shader_type >( )
            );
        }

        auto gl_error = std::vector< char >( static_cast< size_t >( log_length ) );
        glGetShaderInfoLog( id, log_length, nullptr, gl_error.data( ) );

        return LTB_MAKE_UNEXPECTED_ERROR(
            "Shader '{}' ({}): {}",
            filename.filename( ).string( ),
            to_string< shader_type >( ),
            std::string( gl_error.data( ) )
        );
    }

    return utils::success( );
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
