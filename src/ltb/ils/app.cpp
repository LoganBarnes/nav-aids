#include "ltb/ils/app.hpp"

// project
#include "ltb/utils/error_callback.hpp"

// external
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>

namespace ltb::ils
{
namespace
{

constexpr auto color_texture_internal_format = GL_RGBA;
constexpr auto color_texture_format          = GL_RGBA;
constexpr auto color_texture_type            = GL_UNSIGNED_BYTE;

auto store_framebuffer_textures( ogl::Framebuffer& framebuffer ) -> utils::Result<>
{
    auto color_texture = ogl::Texture{ };
    LTB_CHECK( color_texture.initialize( ) );

    filter_and_wrap_tex_parameteri(
        bind< GL_TEXTURE_2D >( color_texture ),
        GL_LINEAR,
        GL_CLAMP_TO_EDGE
    );

    store_framebuffer_texture_2d< GL_FRAMEBUFFER, GL_TEXTURE_2D >(
        framebuffer,
        { color_texture },
        // No depth texture needed
        std::nullopt
    );

    return utils::success( );
}

} // namespace

App::App( window::Window& window )
    : window_( window )
{
}

auto App::initialize( ) -> utils::Result< App* >
{
    LTB_CHECK(
        framebuffer_size_,
        window_.initialize( {
            .title        = "Instrument Landing System",
            .initial_size = glm::ivec2{ 1280, 720 },
        } )
    );
    spdlog::info( "Framebuffer size: {}x{}", framebuffer_size_.x, framebuffer_size_.y );

    LTB_CHECK( ogl_loader_.initialize( ) );
    LTB_CHECK( framebuffer_.initialize( ) );

    LTB_CHECK( store_framebuffer_textures( framebuffer_ ) );
    on_resize( );

    return this;
}

auto App::run( ) -> utils::Result< void >
{
    while ( !window_.should_close( ) )
    {
        window_.poll_events( );

        if ( auto const resized = window_.resized( ) )
        {
            framebuffer_size_ = resized.value( );
            spdlog::info( "Resized to {}x{}", framebuffer_size_.x, framebuffer_size_.y );

            on_resize( );
        }

        render_to_framebuffer( );

        glViewport( 0, 0, framebuffer_size_.x, framebuffer_size_.y );
        glClear( GL_COLOR_BUFFER_BIT );

#if 1
        auto const default_framebuffer = ogl::Framebuffer{ };

        ogl::Framebuffer::blit(
            bind< GL_READ_FRAMEBUFFER >( framebuffer_ ),
            bind< GL_DRAW_FRAMEBUFFER >( default_framebuffer ),
            math::Range2Di{ { 0, 0 }, framebuffer_size_ },
            { { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT0 } },
            GL_COLOR_BUFFER_BIT,
            GL_NEAREST
        );
#endif

        window_.swap_buffers( );
    }

    return utils::success( );
}

auto App::on_resize( ) const -> void
{
    constexpr auto mipmap_level = GLint{ 0 };
    tex_image_2d< void >(
        ogl::bind< GL_TEXTURE_2D >( framebuffer_.data( ).stored_color_textures[ 0 ] ),
        framebuffer_size_,
        nullptr,
        color_texture_internal_format,
        color_texture_format,
        color_texture_type,
        mipmap_level
    );

    LTB_CHECK_OR(
        check_framebuffer_status( bind< GL_FRAMEBUFFER >( framebuffer_ ) ),
        utils::log_error
    );
}

auto App::render_to_framebuffer( ) const -> void
{
    // Orange
    glClearColor( 1.0F, 0.5F, 0.1F, 1.0F );

    auto const bound_framebuffer
        = bind< GL_FRAMEBUFFER >( framebuffer_, ogl::WhenFinished::RestoreNullState );

    glViewport( 0, 0, framebuffer_size_.x, framebuffer_size_.y );
    glClear( GL_COLOR_BUFFER_BIT );

    // Black
    glClearColor( 0.0F, 0.0F, 0.0F, 1.0F );
}

} // namespace ltb::ils
