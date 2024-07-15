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

auto constexpr draw_start_vertex       = 0;
auto constexpr fullscreen_draw_mode    = GL_TRIANGLE_STRIP;
auto constexpr fullscreen_vertex_count = 4;

} // namespace

auto AntennaApp::initialize( glm::ivec2 const framebuffer_size ) -> utils::Result< void >
{
    framebuffer_size_ = framebuffer_size;
    LTB_CHECK( wave_field_chain_.initialize( framebuffer_size_ ) );

    auto const shader_dir = config::shader_dir_path( );

    LTB_CHECK( wave_pipeline_.initialize(
        shader_dir / "fullscreen.vert",
        shader_dir / "wave.frag",
        "state_size",
        "prev_state",
        "curr_state"
    ) );

    LTB_CHECK( antenna_pipeline_.initialize(
        shader_dir / "antenna.vert",
        shader_dir / "antenna.frag",
        "projection_from_world",
        "time_s",
        "frequency_hz"
    ) );

    antennas_ = std::vector{
        Antenna{ .world_position = { -0.01F, -0.5F }, .antenna_power = 100.0F },
        Antenna{ .world_position = { +0.01F, -0.5F }, .antenna_power = 100.0F },
        Antenna{ .world_position = { -0.01F, +0.5F }, .antenna_power = 100.0F },
        Antenna{ .world_position = { +0.01F, +0.5F }, .antenna_power = 100.0F },
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
    wave_field_chain_ = { };
}

auto AntennaApp::resize( glm::ivec2 const framebuffer_size ) -> void
{
    framebuffer_size_ = framebuffer_size;
    LTB_CHECK_OR( wave_field_chain_.resize( framebuffer_size_ ), utils::log_error );
}

auto AntennaApp::update_framebuffer( ) -> void
{
    wave_field_chain_.swap( );

    auto const bound_framebuffer
        = ogl::bind< GL_FRAMEBUFFER >( wave_field_chain_.get_framebuffer< 0 >( ) );

    glViewport( 0, 0, framebuffer_size_.x, framebuffer_size_.y );
    glClear( GL_COLOR_BUFFER_BIT );

    propagate_waves( );
    render_antennas( );
}

auto AntennaApp::propagate_waves( ) -> void
{
    set( std::get< 0 >( wave_pipeline_.uniforms ), glm::vec2( framebuffer_size_ ) );

    auto const& previous_state = wave_field_chain_.get_texture< 2 >( );
    auto const& current_state  = wave_field_chain_.get_texture< 2 >( );

    auto const active_tex_0 = GLint{ 0 };
    auto const active_tex_1 = GLint{ 1 };

    previous_state.active_tex( active_tex_0 );
    current_state.active_tex( active_tex_1 );

    auto const bound_prev_texture = bind< GL_TEXTURE_2D >( previous_state );
    set( std::get< 1 >( wave_pipeline_.uniforms ), bound_prev_texture, active_tex_0 );

    auto const bound_curr_texture = bind< GL_TEXTURE_2D >( current_state );
    set( std::get< 2 >( wave_pipeline_.uniforms ), bound_curr_texture, active_tex_1 );

    ogl::draw(
        ogl::bind( wave_pipeline_.program ),
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
    auto constexpr antenna_frequency_hz = 2.0F;
    set( std::get< 0 >( antenna_pipeline_.uniforms ), proj_from_world );
    set( std::get< 1 >( antenna_pipeline_.uniforms ), elapsed_time_s );
    set( std::get< 2 >( antenna_pipeline_.uniforms ), antenna_frequency_hz );

    auto const bound_program = ogl::bind( antenna_pipeline_.program );
    ogl::draw(
        bound_program,
        bind( antenna_pipeline_.vertex_array ),
        GL_TRIANGLE_STRIP,
        draw_start_vertex,
        static_cast< GLsizei >( antennas_.size( ) )
    );
}

auto AntennaApp::display_wave_field( ) -> void
{
    glViewport( 0, 0, framebuffer_size_.x, framebuffer_size_.y );
    glClear( GL_COLOR_BUFFER_BIT );

    // Render the wave field.
    auto const& current_state = wave_field_chain_.get_texture< 0 >( );
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
