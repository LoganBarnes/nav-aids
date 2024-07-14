#include "ltb/app/ils_app.hpp"

// project
#include "ltb/utils/error_callback.hpp"

// generated
#include "ltb/ltb_config.hpp"

// external
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>

namespace ltb::app
{
namespace
{

constexpr auto color_texture_internal_format = GL_RGBA;
constexpr auto color_texture_format          = GL_RGBA;
constexpr auto color_texture_type            = GL_UNSIGNED_BYTE;

} // namespace

IlsApp::IlsApp( window::Window& window, gui::ImguiSetup& imgui_setup )
    : window_( window )
    , imgui_setup_( imgui_setup )
{
}

auto IlsApp::initialize( ) -> utils::Result< IlsApp* >
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
    LTB_CHECK( imgui_setup_.initialize( ) );

    auto const shader_dir = config::shader_dir_path( );
    LTB_CHECK( pipeline_.initialize( shader_dir / "fullscreen.vert", shader_dir / "tmp.frag" ) );

    color_texture_.initialize( );
    framebuffer_.initialize( );

    // Attach the color texture to the framebuffer.
    constexpr auto null_depth_texture = std::nullopt;
    framebuffer_texture_2d< GL_TEXTURE_2D >(
        bind< GL_FRAMEBUFFER >( framebuffer_ ),
        { color_texture_ },
        // No depth texture needed for 2D rendering.
        null_depth_texture
    );

    // Specify the color texture parameters.
    auto const bound_texture = bind< GL_TEXTURE_2D >( color_texture_ );
    tex_parameteri( bound_texture, ogl::TexParams::filter( ), GL_LINEAR );
    tex_parameteri( bound_texture, ogl::TexParams::wrap( ), GL_CLAMP_TO_EDGE );

    on_resize( );

    glClearColor( 0.0F, 0.0F, 0.0F, 1.0F );
    return this;
}

auto IlsApp::run( ) -> utils::Result< void >
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

        auto const default_framebuffer = ogl::Framebuffer{ };

        ogl::Framebuffer::blit(
            bind< GL_READ_FRAMEBUFFER >( framebuffer_ ),
            bind< GL_DRAW_FRAMEBUFFER >( default_framebuffer ),
            math::Range2Di{ { 0, 0 }, framebuffer_size_ },
            { { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT0 } },
            GL_COLOR_BUFFER_BIT,
            GL_NEAREST
        );

        render_gui( );

        window_.swap_buffers( );
    }

    return utils::success( );
}

auto IlsApp::on_resize( ) const -> void
{
    constexpr auto mipmap_level = GLint{ 0 };
    tex_image_2d< void >(
        ogl::bind< GL_TEXTURE_2D >( color_texture_ ),
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

auto IlsApp::render_to_framebuffer( ) const -> void
{
    auto const bound_framebuffer = bind< GL_FRAMEBUFFER >( framebuffer_ );

    glViewport( 0, 0, framebuffer_size_.x, framebuffer_size_.y );
    glClear( GL_COLOR_BUFFER_BIT );

    auto constexpr start_index  = 0;
    auto constexpr vertex_count = 4;
    ogl::draw(
        bind( pipeline_.program ),
        bind( pipeline_.vertex_array ),
        GL_TRIANGLE_STRIP,
        start_index,
        vertex_count
    );
}

auto IlsApp::render_gui( ) const -> void
{
    imgui_setup_.new_frame( );

    ImGui::ShowDemoWindow( );

    imgui_setup_.render( );
}

} // namespace ltb::app
