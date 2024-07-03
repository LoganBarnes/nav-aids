// ///////////////////////////////////////////////////////////////////////////////////////
// A Logan Thomas Barnes project
// ///////////////////////////////////////////////////////////////////////////////////////
#include "ltb/ogl/opengl_instance.hpp"

// project
#include "ltb/ogl/opengl.hpp"

// external
#include <spdlog/spdlog.h>

namespace ltb::ogl
{
namespace
{

auto default_debug_callback(
    GLenum /*source*/,
    GLenum type,
    GLuint /*id*/,
    GLenum severity,
    GLsizei /*length*/,
    GLchar const* message,
    void const* /*user_param*/
) -> void
{
    if ( type == GL_DEBUG_TYPE_ERROR )
    {
        spdlog::error(
            "GL CALLBACK: {} type = {:#x}, severity = {:#x}, message = {}\n",
            "** GL ERROR **",
            type,
            severity,
            message
        );
    }
}

} // namespace

OpenglInstance::operator bool( ) const
{
    return opengl_ != 0;
}

OpenglInstance::OpenglInstance( int opengl )
    : opengl_( opengl )
{
}

auto OpenglInstance::Builder::set_debug_callback( DebugCallback callback ) -> Builder&
{
    debug_callback_ = callback;
    return *this;
}

auto OpenglInstance::Builder::use_default_debug_callback( ) -> Builder&
{
    debug_callback_ = default_debug_callback;
    return *this;
}

auto OpenglInstance::Builder::build( ) const -> utils::Result< OpenglInstance >
{
    auto opengl = gl3wInit( );
    if ( opengl != 0 )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "Failed to load OpenGL via gl3wInit() (error: {}", opengl );
    }
    spdlog::debug( "OpenGL loaded" );

    auto const* vendor   = reinterpret_cast< char const* >( glGetString( GL_VENDOR ) );
    auto const* renderer = reinterpret_cast< char const* >( glGetString( GL_RENDERER ) );
    auto const* version  = reinterpret_cast< char const* >( glGetString( GL_VERSION ) );

    spdlog::info( "OpenGL Device: {} {}", vendor, renderer );
    spdlog::info( "OpenGL Version: {}", version );

    if ( debug_callback_ )
    {
        if ( !gl3wGetProcAddress( "glDebugMessageCallback" ) )
        {
            return LTB_MAKE_UNEXPECTED_ERROR( "glDebugMessageCallback cannot be loaded with version {}", version );
        }
        glDebugMessageCallback( reinterpret_cast< GLDEBUGPROC >( debug_callback_ ), nullptr );
    }

    return OpenglInstance( opengl );
}

} // namespace ltb::ogl
