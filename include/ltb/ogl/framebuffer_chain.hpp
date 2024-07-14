#pragma once

// project
#include "ltb/ogl/framebuffer.hpp"
#include "ltb/ogl/texture.hpp"

namespace ltb::ogl
{

struct TextureParams
{
    // Texture parameters.
    GLint texture_filter = GL_NEAREST;
    GLint texture_wrap   = GL_REPEAT;

    // GPU storage format.
    GLint internal_format = GL_RGBA32F;

    // Format of initial CPU data.
    GLenum format = GL_RGBA;
    GLenum type   = GL_FLOAT;
};

template < size_t N >
class FramebufferChain
{
public:
    auto initialize( glm::ivec2 size ) -> utils::Result<>;
    auto initialize( glm::ivec2 size, TextureParams params ) -> utils::Result<>;

    auto resize( glm::ivec2 size ) -> utils::Result<>;

    template < size_t index >
        requires( index < N )
    auto get_texture( ) const -> Texture const&;

    template < size_t index >
        requires( index < N )
    auto get_framebuffer( ) const -> Framebuffer const&;

    auto swap( ) -> void;

private:
    TextureParams                params_;
    std::array< Texture, N >     textures_;
    std::array< Framebuffer, N > framebuffers_;
};

template < size_t N >
auto FramebufferChain< N >::initialize( glm::ivec2 size ) -> utils::Result<>
{
    return initialize( size, TextureParams{ } );
}

template < size_t N >
auto FramebufferChain< N >::initialize( glm::ivec2 size, TextureParams params ) -> utils::Result<>
{
    params_ = params;

    for ( auto i = 0UZ; i < N; ++i )
    {
        textures_[ i ].initialize( );
        framebuffers_[ i ].initialize( );

        // Specify the color texture parameters.
        auto const bound_texture = bind< GL_TEXTURE_2D >( textures_[ i ] );
        tex_parameteri( bound_texture, ogl::TexParams::filter( ), params_.texture_filter );
        tex_parameteri( bound_texture, ogl::TexParams::wrap( ), params_.texture_wrap );
    }

    return resize( size );
}

template < size_t N >
auto FramebufferChain< N >::resize( glm::ivec2 size ) -> utils::Result<>
{
    for ( auto i = 0UZ; i < N; ++i )
    {
        constexpr auto mipmap_level = GLint{ 0 };
        tex_image_2d< void >(
            ogl::bind< GL_TEXTURE_2D >( textures_[ i ] ),
            size,
            nullptr,
            params_.internal_format,
            params_.format,
            params_.type,
            mipmap_level
        );

        // Attach the color texture to the framebuffer.
        constexpr auto null_depth_texture = std::nullopt;
        framebuffer_texture_2d< GL_TEXTURE_2D >(
            bind< GL_FRAMEBUFFER >( framebuffers_[ i ] ),
            { textures_[ i ] },
            // No depth texture needed for 2D rendering.
            null_depth_texture
        );

        LTB_CHECK( check_framebuffer_status( bind< GL_FRAMEBUFFER >( framebuffers_[ i ] ) ) );
    }

    return utils::success( );
}

template < size_t N >
template < size_t index >
    requires( index < N )
auto FramebufferChain< N >::get_texture( ) const -> Texture const&
{
    return textures_[ index ];
}

template < size_t N >
template < size_t index >
    requires( index < N )
auto FramebufferChain< N >::get_framebuffer( ) const -> Framebuffer const&
{
    return framebuffers_[ index ];
}

template < size_t N >
auto FramebufferChain< N >::swap( ) -> void
{
    // [a, b, c] -> [c, a, b]
    for ( size_t i = N; i > 1UZ; --i )
    {
        std::swap( textures_[ i - 1UZ ], textures_[ i - 2UZ ] );
        std::swap( framebuffers_[ i - 1UZ ], framebuffers_[ i - 2UZ ] );
    }
}

} // namespace ltb::ogl
