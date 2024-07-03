// ///////////////////////////////////////////////////////////////////////////////////////
// A Logan Thomas Barnes project
// ///////////////////////////////////////////////////////////////////////////////////////
#include "ltb/ogl/utils.hpp"

// standard
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
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
}

auto binding_type( GLenum type ) -> GLenum
{
    switch ( type )
    {
            // Buffers
        case GL_ARRAY_BUFFER:
            return GL_ARRAY_BUFFER_BINDING;
        case GL_ATOMIC_COUNTER_BUFFER:
            return GL_ATOMIC_COUNTER_BUFFER_BINDING;
        case GL_COPY_READ_BUFFER:
            return GL_COPY_READ_BUFFER_BINDING;
        case GL_COPY_WRITE_BUFFER:
            return GL_COPY_WRITE_BUFFER_BINDING;
        case GL_DRAW_INDIRECT_BUFFER:
            return GL_DRAW_INDIRECT_BUFFER_BINDING;
        case GL_DISPATCH_INDIRECT_BUFFER:
            return GL_DISPATCH_INDIRECT_BUFFER_BINDING;
        case GL_ELEMENT_ARRAY_BUFFER:
            return GL_ELEMENT_ARRAY_BUFFER_BINDING;
        case GL_PIXEL_PACK_BUFFER:
            return GL_PIXEL_PACK_BUFFER_BINDING;
        case GL_PIXEL_UNPACK_BUFFER:
            return GL_PIXEL_UNPACK_BUFFER_BINDING;
        case GL_SHADER_STORAGE_BUFFER:
            return GL_SHADER_STORAGE_BUFFER_BINDING;
        case GL_TRANSFORM_FEEDBACK_BUFFER:
            return GL_TRANSFORM_FEEDBACK_BUFFER_BINDING;
        case GL_UNIFORM_BUFFER:
            return GL_UNIFORM_BUFFER_BINDING;

            // Textures
        case GL_TEXTURE_1D:
            return GL_TEXTURE_BINDING_1D;
        case GL_TEXTURE_2D:
            return GL_TEXTURE_BINDING_2D;
        case GL_TEXTURE_3D:
            return GL_TEXTURE_BINDING_3D;
        case GL_TEXTURE_1D_ARRAY:
            return GL_TEXTURE_BINDING_1D_ARRAY;
        case GL_TEXTURE_2D_ARRAY:
            return GL_TEXTURE_BINDING_2D_ARRAY;
        case GL_TEXTURE_RECTANGLE:
            return GL_TEXTURE_BINDING_RECTANGLE;
        case GL_TEXTURE_CUBE_MAP:
            return GL_TEXTURE_BINDING_CUBE_MAP;
        case GL_TEXTURE_CUBE_MAP_ARRAY:
            return GL_TEXTURE_BINDING_CUBE_MAP_ARRAY;
        case GL_TEXTURE_BUFFER:
            return GL_TEXTURE_BINDING_BUFFER;
        case GL_TEXTURE_2D_MULTISAMPLE:
            return GL_TEXTURE_BINDING_2D_MULTISAMPLE;
        case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
            return GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY;

            // Framebuffer
        case GL_FRAMEBUFFER:
            return GL_FRAMEBUFFER_BINDING;
        case GL_DRAW_FRAMEBUFFER:
            return GL_DRAW_FRAMEBUFFER_BINDING;
        case GL_READ_FRAMEBUFFER:
            return GL_READ_FRAMEBUFFER_BINDING;

        default:
            break;
    }
    throw std::runtime_error( "The corresponding binding for this type has not been programmed, sorry :/" );
}

DestructionCallback::DestructionCallback( std::function< void( ) > callback )
    : callback_( std::move( callback ) )
{
}

DestructionCallback::~DestructionCallback( )
{
    if ( callback_ )
    {
        callback_( );
    }
}

} // namespace ltb::ogl
