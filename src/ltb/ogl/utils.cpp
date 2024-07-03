// ///////////////////////////////////////////////////////////////////////////////////////
// A Logan Thomas Barnes project
// ///////////////////////////////////////////////////////////////////////////////////////
#include "ltb/ogl/utils.hpp"

// standard
#include <sstream>
#include <stdexcept>

namespace ltb::ogl
{

auto set_defaults( ) -> void
{
    // Enable depth testing, so that objects are occluded based on depth instead of drawing order.
    glEnable( GL_DEPTH_TEST );

    // IBOs can use gl::restart_index() to create a break in the drawing cycle
    glEnable( GL_PRIMITIVE_RESTART );
    glPrimitiveRestartIndex( default_restart_index( ) );

    // point size can be specified in the vertex shader
    glEnable( GL_PROGRAM_POINT_SIZE );

    // Move the polygons back a bit so lines are still drawn even though they are coplanar with the
    // polygons they came from, which will be drawn before them.
    glEnable( GL_POLYGON_OFFSET_LINE );
    glPolygonOffset( -1, -1 );

    // Enable back-face culling, meaning only the front side of every face is rendered.
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );

    // Specify that the front face is represented by vertices in counterclockwise order (this is
    // the default).
    glFrontFace( GL_CCW );

    // Specify the color used when glClear is called
    glClearColor( 0.0F, 0.0F, 0.0F, 1.0F );
}

auto to_string( GLubyte const* const str ) -> std::string
{
    if ( nullptr == str )
    {
        return { };
    }

    // Technically, we could reinterpret_cast to a `char const*`, but this is safer.
    auto convert = std::ostringstream{ };

    // The max size limitation prevents infinite loops.
    constexpr auto max_string_size = 512;
    for ( auto i = 0; ( str[ i ] != 0 ) && ( i < max_string_size ); ++i )
    {
        convert << str[ i ];
    }
    return convert.str( );
}

auto get_string( GLenum const name ) -> std::string
{
    return to_string( glGetString( name ) );
}

} // namespace ltb::ogl
