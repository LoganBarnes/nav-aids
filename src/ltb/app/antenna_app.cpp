#include "ltb/app/antenna_app.hpp"

// project
#include "ltb/utils/error_callback.hpp"

// generated
#include "ltb/ltb_config.hpp"

// external
#include <glm/gtc/matrix_transform.hpp>

namespace ltb::app
{
namespace
{

auto constexpr wave_texture_internal_format = GL_RGBA32F;
auto constexpr wave_texture_format          = GL_RGBA;
auto constexpr wave_texture_type            = GL_FLOAT;

auto constexpr wave_texture_filter = GL_NEAREST;
auto constexpr wave_texture_wrap   = GL_CLAMP_TO_EDGE;

auto constexpr draw_start_vertex       = 0;
auto constexpr fullscreen_draw_mode    = GL_TRIANGLE_STRIP;
auto constexpr fullscreen_vertex_count = 4;

} // namespace

auto AntennaApp::initialize( glm::ivec2 const framebuffer_size ) -> utils::Result< void >
{
    for ( auto i = 0UZ; i < framebuffer_count_; ++i )
    {
        wave_field_textures_[ i ].initialize( );
        wave_field_framebuffers_[ i ].initialize( );

        // Specify the color texture parameters.
        auto const bound_texture = bind< GL_TEXTURE_2D >( wave_field_textures_[ i ] );
        tex_parameteri( bound_texture, ogl::TexParams::filter( ), wave_texture_filter );
        tex_parameteri( bound_texture, ogl::TexParams::wrap( ), wave_texture_wrap );
    }

    auto const shader_dir = config::shader_dir_path( );

    LTB_CHECK( wave_pipeline_.initialize(
        shader_dir / "fullscreen.vert",
        shader_dir / "wave.frag",
        "previous_state"
    ) );

    LTB_CHECK( antenna_pipeline_.initialize(
        shader_dir / "antenna.vert",
        shader_dir / "antenna.frag",
        "projection_from_world",
        "time"
    ) );

    antennas_ = std::vector{
        Antenna{ .world_position = { 0.0F, -0.5F }, .antenna_power = 1.0F },
        Antenna{ .world_position = { 0.0F, +0.5F }, .antenna_power = 1.0F },
    };

    // Store the vertex data in a GPU buffer.
    ogl::buffer_data(
        ogl::bind< GL_ARRAY_BUFFER >( antenna_pipeline_.vertex_buffer ),
        antennas_,
        GL_STATIC_DRAW
    );

    LTB_CHECK(
        auto const world_position_attrib,
        antenna_pipeline_.program.get_attribute_location( "world_position" )
    );
    LTB_CHECK(
        auto const antenna_power_attrib,
        antenna_pipeline_.program.get_attribute_location( "antenna_power" )
    );

    constexpr Antenna const* const null_antenna_ptr = nullptr;
    // Tightly packed.
    constexpr auto total_vertex_stride = sizeof( Antenna );
    // Not instanced
    constexpr auto attrib_divisor = 0U;

    ogl::set_attributes< void >(
        ogl::bind( antenna_pipeline_.vertex_array ),
        ogl::bind< GL_ARRAY_BUFFER >( antenna_pipeline_.vertex_buffer ),
        {
            {
                .attribute_location      = world_position_attrib,
                .num_coordinates         = glm::vec2::length( ),
                .data_type               = GL_FLOAT,
                .initial_offset_into_vbo = &( null_antenna_ptr->world_position ),
            },
            {
                .attribute_location      = antenna_power_attrib,
                .num_coordinates         = 1,
                .data_type               = GL_FLOAT,
                .initial_offset_into_vbo = &( null_antenna_ptr->antenna_power ),
            },
        },
        total_vertex_stride,
        attrib_divisor
    );

    LTB_CHECK( display_pipeline_.initialize(
        shader_dir / "fullscreen.vert",
        shader_dir / "wave_display.frag",
        "wave_texture"
    ) );

    resize( framebuffer_size );

    glClearColor( 0.0F, 0.0F, 0.0F, 1.0F );
    glDisable( GL_DEPTH_TEST );

    start_time_ = std::chrono::steady_clock::now( );
    return utils::success( );
}

auto AntennaApp::render( ) -> void
{
    update_framebuffer( );
    display_wave_field( );
}

auto AntennaApp::configure_gui( ) -> void {}

auto AntennaApp::destroy( ) -> void
{
    display_pipeline_.destroy( );
    antennas_ = { };
    antenna_pipeline_.destroy( );
    wave_pipeline_.destroy( );
    wave_field_framebuffers_ = { };
    wave_field_textures_     = { };
}

auto AntennaApp::resize( glm::ivec2 const framebuffer_size ) -> void
{
    for ( auto i = 0UZ; i < framebuffer_count_; ++i )
    {
        constexpr auto mipmap_level = GLint{ 0 };
        tex_image_2d< void >(
            ogl::bind< GL_TEXTURE_2D >( wave_field_textures_[ i ] ),
            framebuffer_size,
            nullptr,
            wave_texture_internal_format,
            wave_texture_format,
            wave_texture_type,
            mipmap_level
        );

        // Attach the color texture to the framebuffer.
        constexpr auto null_depth_texture = std::nullopt;
        framebuffer_texture_2d< GL_TEXTURE_2D >(
            bind< GL_FRAMEBUFFER >( wave_field_framebuffers_[ i ] ),
            { wave_field_textures_[ i ] },
            // No depth texture needed for 2D rendering.
            null_depth_texture
        );

        LTB_CHECK_OR(
            check_framebuffer_status( bind< GL_FRAMEBUFFER >( wave_field_framebuffers_[ i ] ) ),
            utils::log_error
        );
    }
}

auto AntennaApp::update_framebuffer( ) -> void
{
    previous_wave_field_ = current_wave_field_;
    current_wave_field_  = ( current_wave_field_ + 1UZ ) % framebuffer_count_;

    auto const bound_framebuffer
        = ogl::bind< GL_FRAMEBUFFER >( wave_field_framebuffers_[ current_wave_field_ ] );

    auto const viewport_size    = ImGui::GetMainViewport( )->Size;
    auto const framebuffer_size = glm::ivec2{
        static_cast< int32 >( viewport_size.x ),
        static_cast< int32 >( viewport_size.y ),
    };

    glViewport( 0, 0, framebuffer_size.x, framebuffer_size.y );
    glClear( GL_COLOR_BUFFER_BIT );

    propagate_waves( );
    render_antennas( );
}

auto AntennaApp::propagate_waves( ) -> void
{
    auto const& previous_state = wave_field_textures_[ previous_wave_field_ ];
    auto const  active_tex     = GLint{ 0 };
    previous_state.active_tex( active_tex );

    auto const bound_texture = bind< GL_TEXTURE_2D >( previous_state );
    set( std::get< 0 >( wave_pipeline_.uniforms ), bound_texture, active_tex );

    auto const bound_program = ogl::bind( wave_pipeline_.program );
    ogl::draw(
        bound_program,
        bind( wave_pipeline_.vertex_array ),
        fullscreen_draw_mode,
        draw_start_vertex,
        fullscreen_vertex_count
    );
}

auto AntennaApp::render_antennas( ) -> void
{
    // Time since start in seconds.
    using Duration = std::chrono::duration< float32 >;

    auto const current_time     = std::chrono::steady_clock::now( );
    auto const elapsed_duration = current_time - start_time_;
    auto const elapsed_time_s = std::chrono::duration_cast< Duration >( elapsed_duration ).count( );

    // Camera projection matrix.
    auto constexpr proj_from_world = glm::identity< glm::mat4 >( );

    // Render the wave field.
    set( std::get< 0 >( antenna_pipeline_.uniforms ), proj_from_world );
    set( std::get< 1 >( antenna_pipeline_.uniforms ), elapsed_time_s );

    auto const bound_program = ogl::bind( antenna_pipeline_.program );
    ogl::draw(
        bound_program,
        bind( antenna_pipeline_.vertex_array ),
        GL_LINES,
        draw_start_vertex,
        static_cast< GLsizei >( antennas_.size( ) )
    );
}

auto AntennaApp::display_wave_field( ) -> void
{
    auto const viewport_size    = ImGui::GetMainViewport( )->Size;
    auto const framebuffer_size = glm::ivec2{
        static_cast< int32 >( viewport_size.x ),
        static_cast< int32 >( viewport_size.y ),
    };

    glViewport( 0, 0, framebuffer_size.x, framebuffer_size.y );
    glClear( GL_COLOR_BUFFER_BIT );

    // Render the wave field.
    auto const& current_state = wave_field_textures_[ current_wave_field_ ];
    auto const  active_tex    = GLint{ 0 };
    current_state.active_tex( active_tex );

    auto const bound_texture = bind< GL_TEXTURE_2D >( current_state );
    set( std::get< 0 >( display_pipeline_.uniforms ), bound_texture, active_tex );

    ogl::draw(
        bind( display_pipeline_.program ),
        bind( display_pipeline_.vertex_array ),
        fullscreen_draw_mode,
        draw_start_vertex,
        fullscreen_vertex_count
    );
}

} // namespace ltb::app
