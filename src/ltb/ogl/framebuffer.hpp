#pragma once

// graphics
#include "opengl.hpp"

// project
#include "ltb/math/range.hpp"
#include "ltb/ogl/fwd.hpp"
#include "ltb/ogl/texture.hpp"
#include "ltb/ogl/utils.hpp"
#include "ltb/utils/types.hpp"

// standard
#include <memory>
#include <optional>
#include <vector>

namespace ltb::ogl
{

struct FramebufferAttachmentPair
{
    std::optional< GLenum > read  = std::nullopt;
    std::optional< GLenum > write = std::nullopt;
};

/// \brief All the data the Framebuffer class stores and manages.
struct FramebufferData
{
    /// \brief OpenGL ID from `glGenFramebuffers()`
    GLuint gl_id = 0u;
};

class Framebuffer
{
public:
    Framebuffer( )          = default;
    virtual ~Framebuffer( ) = default;

    /// \brief Initialize the framebuffer object. This must
    ///        be called before using the framebuffer.
    auto initialize( ) -> utils::Result<>;

    /// \brief The raw settings stored for this framebuffer
    [[nodiscard( "Const getter" )]]
    auto data( ) const -> FramebufferData const&;

    static auto static_bind( GLenum type, Framebuffer const& framebuffer ) -> void;
    static auto static_bind( GLenum type, GLuint raw_gl_id ) -> void;

    /// \brief Copy the contents of one framebuffer to another.
    /// \code
    /// ogl::Framebuffer::blit(read_framebuffer,
    ///                        draw_framebuffer,
    ///                        {{0, 0}, {800, 600}},
    ///                        {{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT0}},
    ///                        GL_COLOR_BUFFER_BIT,
    ///                        GL_NEAREST);
    /// \endcode
    static auto blit(
        Bound< Framebuffer, GL_READ_FRAMEBUFFER > const& read_framebuffer,
        Bound< Framebuffer, GL_DRAW_FRAMEBUFFER > const& draw_framebuffer,
        math::Range2Di const&                            viewport,
        std::vector< FramebufferAttachmentPair > const&  attachments,
        GLbitfield                                       mask,
        GLenum                                           filter
    ) -> void;

    /// \brief Read the pixels from the currently bound framebuffer.
    /// \code
    /// ogl::Framebuffer::read_pixels(GL_COLOR_ATTACHMENT1,
    ///                               {{mouse_position.x, mouse_position.y}, {1, 1}},
    ///                               GL_RED_INTEGER,
    ///                               GL_UNSIGNED_INT);
    /// \endcode
    template < typename PixelData >
    static auto read_pixels(
        GLenum                attachment,
        math::Range2Di const& viewport,
        GLenum                format,
        GLenum                type,
        PixelData*            pixels
    ) -> void;

private:
    FramebufferData         data_    = { };
    std::shared_ptr< void > deleter_ = nullptr;
};

template < typename PixelData >
auto Framebuffer::read_pixels(
    GLenum const          attachment,
    math::Range2Di const& viewport,
    GLenum const          format,
    GLenum const          type,
    PixelData* const      pixels
) -> void
{
    auto const size = dimensions( viewport );
    glReadBuffer( attachment );
    glReadPixels( viewport.min.x, viewport.min.y, size.x, size.y, format, type, pixels );
}

/// \brief Attach a texture to the framebuffer.
template < GLenum tex_bind_type, GLenum bind_type >
auto framebuffer_texture_2d(
    Bound< Framebuffer, bind_type > const& bound_framebuffer,
    GLenum const                           attachment,
    Bound< Texture, tex_bind_type > const& bound_texture
) -> void
{
    // This argument is provided to ensure the framebuffer
    // is bound, but nothing has to be done with it.
    utils::ignore( bound_framebuffer );

    auto const& texture = bound_texture.object( );

    // From the OpenGL documentation:
    // "Specifies the mipmap level of the texture image to be attached, which must be 0."
    // https://registry.khronos.org/OpenGL-Refpages/es2.0/xhtml/glFramebufferTexture2D.xml
    constexpr auto level = 0;

    glFramebufferTexture2D( bind_type, attachment, tex_bind_type, texture.data( ).gl_id, level );
}

/// \brief Attach color textures and an optional depth texture to the framebuffer.
template < GLenum tex_bind_type, GLenum bind_type >
auto framebuffer_texture_2d(
    Bound< Framebuffer, bind_type > const& bound_framebuffer,
    std::vector< Texture >                 color_textures,
    std::optional< Texture >               optional_depth_texture
) -> void
{
    auto const texture_count = color_textures.size( );
    for ( auto i = 0U; i < texture_count; ++i )
    {
        framebuffer_texture_2d(
            bound_framebuffer,
            GL_COLOR_ATTACHMENT0 + i,
            bind< tex_bind_type >( color_textures[ i ] )
        );
    }

    if ( optional_depth_texture.has_value( ) )
    {
        framebuffer_texture_2d(
            bound_framebuffer,
            GL_DEPTH_ATTACHMENT,
            bind< tex_bind_type >( optional_depth_texture.value( ) )
        );
    }
}

/// \brief Check the status of the currently bound framebuffer.
template < GLenum bind_type >
auto check_framebuffer_status( Bound< Framebuffer, bind_type > const& bound_framebuffer
) -> utils::Result<>
{
    // This argument is provided to ensure the framebuffer
    // is bound, but nothing has to be done with it.
    utils::ignore( bound_framebuffer );

    auto const status = glCheckFramebufferStatus( GL_FRAMEBUFFER );

    // The error messages were copied word for word from:
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glCheckFramebufferStatus.xhtml

    switch ( status )
    {
        case GL_FRAMEBUFFER_COMPLETE:
            return utils::success( );

        case GL_FRAMEBUFFER_UNDEFINED:
            return LTB_MAKE_UNEXPECTED_ERROR(
                "GL_FRAMEBUFFER_UNDEFINED is returned if the specified framebuffer is the default "
                "read or draw framebuffer, but the default framebuffer does not exist"
            );

        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            return LTB_MAKE_UNEXPECTED_ERROR(
                "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT is returned if any of the framebuffer "
                "attachment points are framebuffer incomplete"
            );

        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            return LTB_MAKE_UNEXPECTED_ERROR(
                "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT is returned if the framebuffer does "
                "not have at least one image attached to it"
            );

        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            return LTB_MAKE_UNEXPECTED_ERROR(
                "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER is returned if the value of "
                "GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment "
                "point(s) named by GL_DRAW_BUFFERi"
            );

        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            return LTB_MAKE_UNEXPECTED_ERROR(
                "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER is returned if GL_READ_BUFFER is not "
                "GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPEis GL_NONE for the "
                "color attachment point named by GL_READ_BUFFER"
            );

        case GL_FRAMEBUFFER_UNSUPPORTED:
            return LTB_MAKE_UNEXPECTED_ERROR(
                "GL_FRAMEBUFFER_UNSUPPORTED is returned if the combination of internal formats of "
                "the attached images violates an implementation-dependent set of restrictions"
            );

        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            return LTB_MAKE_UNEXPECTED_ERROR(
                "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE is returned if the value of "
                "GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers; if the "
                "value of GL_TEXTURE_SAMPLES is the not same for all attached textures; or, if the "
                "attached images are a mix of renderbuffers and textures, the value of "
                "GL_RENDERBUFFER_SAMPLES does not match the value of "
                "GL_TEXTURE_SAMPLES\nGL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE is also returned if the "
                "value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all attached "
                "textures; or, if the attached images are a mix of renderbuffers and textures, the "
                "value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE for all attached "
                "textures"
            );

        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            return LTB_MAKE_UNEXPECTED_ERROR(
                "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS is returned if any framebuffer attachment "
                "is layered, and any populated attachment is not layered, or if all populated "
                "color attachments are not from textures of the same target"
            );

        default:
            break;
    }

    return LTB_MAKE_UNEXPECTED_ERROR(
        "Failed to create framebuffer for unknown reason. Error code: 0x{:0X}",
        status
    );
}

} // namespace ltb::ogl
