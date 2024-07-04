#include "ltb/ogl/framebuffer.hpp"

// project
#include "ltb/ogl/object_id.hpp"

// external
#include <spdlog/fmt/fmt.h>

namespace ltb::ogl
{

auto Framebuffer::initialize( ) -> void
{
    glGenFramebuffers( 1, &data_.gl_id );

    spdlog::debug( "glGenFramebuffers({})", data_.gl_id );
    deleter_ = make_array_deleter( { data_.gl_id }, glDeleteFramebuffers, "glDeleteFramebuffers" );
}

auto Framebuffer::data( ) const -> FramebufferData const&
{
    return data_;
}

auto Framebuffer::static_bind( GLenum const type, Framebuffer const& framebuffer ) -> void
{
    Framebuffer::static_bind( type, framebuffer.data( ).gl_id );
}

auto Framebuffer::static_bind( GLenum const type, GLuint const raw_gl_id ) -> void
{
    glBindFramebuffer( type, raw_gl_id );
}

auto Framebuffer::blit(
    Bound< Framebuffer, GL_READ_FRAMEBUFFER > const& read_framebuffer,
    Bound< Framebuffer, GL_DRAW_FRAMEBUFFER > const& draw_framebuffer,
    math::Range2Di const&                            viewport,
    std::vector< FramebufferAttachmentPair > const&  attachments,
    GLbitfield const                                 mask,
    GLenum const                                     filter
) -> void
{
    // These need to be bound, but nothing has to be done with them
    utils::ignore( read_framebuffer, draw_framebuffer );

    for ( auto attachment : attachments )
    {
        // Specify which attachments should be used (GL_COLOR_ATTACHMENT[0,1,...])
        if ( attachment.read )
        {
            glReadBuffer( attachment.read.value( ) );
        }
        if ( attachment.write )
        {
            glDrawBuffer( attachment.write.value( ) );
        }

        // Copy from one buffer to the other
        glBlitFramebuffer(
            viewport.min.x,
            viewport.min.y,
            viewport.max.x,
            viewport.max.y,
            viewport.min.x,
            viewport.min.y,
            viewport.max.x,
            viewport.max.y,
            mask,
            filter
        );
    }
}

} // namespace ltb::ogl
