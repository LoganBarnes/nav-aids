// ///////////////////////////////////////////////////////////////////////////////////////
// A Logan Thomas Barnes project
// ///////////////////////////////////////////////////////////////////////////////////////
#include "ltb/ogl/opengl_loader.hpp"

// project
#include "ltb/ogl/utils.hpp"

// external
#include <spdlog/spdlog.h>

namespace ltb::ogl
{
namespace
{

auto default_debug_callback(
    GLenum const        source,
    GLenum const        type,
    GLuint const        id,
    GLenum const        severity,
    GLsizei const       length,
    GLchar const* const message,
    void const* const   user_param
) -> void
{
    utils::ignore( source, id, length, user_param );

    if ( GL_DEBUG_TYPE_ERROR == type )
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

OpenglLoader::operator bool( ) const
{
    return 0 != opengl_;
}

auto OpenglLoader::initialize( ) -> utils::Result<>
{
    if ( opengl_ = ::gl3wInit( ); 0 != opengl_ )
    {
        return LTB_MAKE_UNEXPECTED_ERROR(
            "Failed to load OpenGL via gl3wInit() (error: {}",
            opengl_
        );
    }
    spdlog::debug( "OpenGL loaded" );

    auto const vendor   = ogl::get_string( GL_VENDOR );
    auto const renderer = ogl::get_string( GL_RENDERER );
    auto const version  = ogl::get_string( GL_VERSION );

    spdlog::info( "OpenGL Device: {} {}", vendor, renderer );
    spdlog::info( "OpenGL Version: {}", version );

    return utils::success( );
}

auto OpenglLoader::initialize( GLDEBUGPROC const debug_callback ) -> utils::Result<>
{
    LTB_CHECK( initialize( ) );

    if ( nullptr == ::gl3wGetProcAddress( "glDebugMessageCallback" ) )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "glDebugMessageCallback cannot be loaded" );
    }

    if ( nullptr != debug_callback )
    {
        glDebugMessageCallback( debug_callback, nullptr );
    }
    else
    {
        glDebugMessageCallback( &default_debug_callback, nullptr );
    }

    return utils::success( );
}

} // namespace ltb::ogl
