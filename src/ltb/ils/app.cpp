#include "ltb/ils/app.hpp"

// project
#include "ltb/utils/error_callback.hpp"

// generated
#include "ltb/ltb_config.hpp"

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

    LTB_CHECK( vertex_shader_.initialize( ) );
    LTB_CHECK( fragment_shader_.initialize( ) );
    LTB_CHECK( program_.initialize( ) );
    vertex_buffer_.initialize( );
    vertex_array_.initialize( );
    color_texture_.initialize( );
    framebuffer_.initialize( );

    auto const shader_dir = config::shader_dir_path( );

    LTB_CHECK( vertex_shader_.load_and_compile( shader_dir / "tmp.vert" ) );
    LTB_CHECK( fragment_shader_.load_and_compile( shader_dir / "tmp.frag" ) );
    LTB_CHECK( program_.attach_and_link( vertex_shader_, fragment_shader_ ) );

    // Store the vertex data in a GPU buffer.
    ogl::buffer_data(
        ogl::bind< GL_ARRAY_BUFFER >( vertex_buffer_ ),
        std::vector< glm::vec2 >{
            { -1.0F, -1.0F },
            { +1.0F, -1.0F },
            { -1.0F, +1.0F },
            { +1.0F, +1.0F },
        },
        GL_STATIC_DRAW
    );

    // Tightly packed.
    constexpr auto total_vertex_stride = 0U;
    // Not instanced
    constexpr auto attrib_divisor = 0U;

    ogl::set_attributes< glm::vec2 >(
        ogl::bind( vertex_array_ ),
        ogl::bind< GL_ARRAY_BUFFER >( vertex_buffer_ ),
        { {
            .attribute_location      = 0,
            .num_coordinates         = glm::vec2::length( ),
            .data_type               = GL_FLOAT,
            .initial_offset_into_vbo = nullptr,
        } },
        total_vertex_stride,
        attrib_divisor
    );

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

        auto const default_framebuffer = ogl::Framebuffer{ };

        ogl::Framebuffer::blit(
            bind< GL_READ_FRAMEBUFFER >( framebuffer_ ),
            bind< GL_DRAW_FRAMEBUFFER >( default_framebuffer ),
            math::Range2Di{ { 0, 0 }, framebuffer_size_ },
            { { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT0 } },
            GL_COLOR_BUFFER_BIT,
            GL_NEAREST
        );

        window_.swap_buffers( );
    }

    return utils::success( );
}

auto App::on_resize( ) const -> void
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

auto App::render_to_framebuffer( ) const -> void
{
    auto const bound_framebuffer = bind< GL_FRAMEBUFFER >( framebuffer_ );

    glViewport( 0, 0, framebuffer_size_.x, framebuffer_size_.y );
    glClear( GL_COLOR_BUFFER_BIT );

    auto constexpr start_index  = 0;
    auto constexpr vertex_count = 4;
    ogl::draw(
        bind( program_ ),
        bind( vertex_array_ ),
        GL_TRIANGLE_STRIP,
        start_index,
        vertex_count
    );
}

} // namespace ltb::ils
