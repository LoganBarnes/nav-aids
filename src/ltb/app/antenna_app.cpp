#include "ltb/app/antenna_app.hpp"

// project
#include "ltb/ogl/program_attribute.hpp"
#include "ltb/utils/error_callback.hpp"

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

    LTB_CHECK( ogl::initialize(
        wave_pipeline_.vertex_shader,
        wave_pipeline_.fragment_shader,
        wave_pipeline_.program,
        wave_pipeline_.state_size_uniform,
        wave_pipeline_.prev_state_uniform,
        wave_pipeline_.curr_state_uniform,
        wave_pipeline_.vertex_array
    ) );
    LTB_CHECK( ogl::initialize(
        antenna_pipeline_.vertex_shader,
        antenna_pipeline_.fragment_shader,
        antenna_pipeline_.program,
        antenna_pipeline_.clip_from_world_uniform,
        antenna_pipeline_.time_s_uniform,
        antenna_pipeline_.frequency_hz_uniform,
        antenna_pipeline_.vertex_buffer,
        antenna_pipeline_.vertex_array
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

    constexpr Antenna const* const null_antenna_ptr = nullptr;
    // Tightly packed.
    constexpr auto total_vertex_stride = sizeof( Antenna );
    // Not instanced
    constexpr auto attrib_divisor = 0U;

    auto antenna_world_position_attrib = ogl::Attribute< decltype( Antenna::world_position ) >{
        antenna_pipeline_.program,
        "world_position",
    };
    auto antenna_power_attrib = ogl::Attribute< decltype( Antenna::antenna_power ) >{
        antenna_pipeline_.program,
        "antenna_power",
    };
    LTB_CHECK( ogl::initialize( antenna_world_position_attrib, antenna_power_attrib ) );

    ogl::set_attributes< void >(
        ogl::bind( antenna_pipeline_.vertex_array ),
        ogl::bind< GL_ARRAY_BUFFER >( antenna_pipeline_.vertex_buffer ),
        {
            {
                .attribute_location      = antenna_world_position_attrib.location( ),
                .num_coordinates         = decltype( null_antenna_ptr->world_position )::length( ),
                .data_type               = GL_FLOAT,
                .initial_offset_into_vbo = &( null_antenna_ptr->world_position ),
            },
            {
                .attribute_location      = antenna_power_attrib.location( ),
                .num_coordinates         = 1,
                .data_type               = GL_FLOAT,
                .initial_offset_into_vbo = &( null_antenna_ptr->antenna_power ),
            },
        },
        total_vertex_stride,
        attrib_divisor
    );

    LTB_CHECK( ogl::initialize(
        display_pipeline_.vertex_shader,
        display_pipeline_.fragment_shader,
        display_pipeline_.program,
        display_pipeline_.wave_texture_uniform,
        display_pipeline_.vertex_array
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
    display_pipeline_.program = { };

    antennas_ = { };

    antenna_pipeline_.vertex_array  = { };
    antenna_pipeline_.vertex_buffer = { };
    antenna_pipeline_.program       = { };

    wave_pipeline_.program = { };

    wave_field_chain_ = { };

    fullscreen_vertex_array_ = { };
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
    ogl::set( wave_pipeline_.state_size_uniform, glm::vec2( framebuffer_size_ ) );

    auto const& previous_state = wave_field_chain_.get_texture< 1 >( );
    auto const& current_state  = wave_field_chain_.get_texture< 2 >( );

    auto const active_tex_0 = GLint{ 0 };
    auto const active_tex_1 = GLint{ 1 };

    previous_state.active_tex( active_tex_0 );
    current_state.active_tex( active_tex_1 );

    auto const bound_prev_texture = bind< GL_TEXTURE_2D >( previous_state );
    ogl::set( wave_pipeline_.prev_state_uniform, bound_prev_texture, active_tex_0 );

    auto const bound_curr_texture = bind< GL_TEXTURE_2D >( current_state );
    ogl::set( wave_pipeline_.curr_state_uniform, bound_curr_texture, active_tex_1 );

    ogl::draw(
        ogl::bind( wave_pipeline_.program ),
        ogl::bind( wave_pipeline_.vertex_array ),
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
    ogl::set( antenna_pipeline_.clip_from_world_uniform, proj_from_world );
    ogl::set( antenna_pipeline_.time_s_uniform, elapsed_time_s );
    ogl::set( antenna_pipeline_.frequency_hz_uniform, antenna_frequency_hz );

    ogl::draw(
        ogl::bind( antenna_pipeline_.program ),
        ogl::bind( antenna_pipeline_.vertex_array ),
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
    ogl::set( display_pipeline_.wave_texture_uniform, bound_texture, active_tex );

    ogl::draw(
        ogl::bind( display_pipeline_.program ),
        ogl::bind( display_pipeline_.vertex_array ),
        fullscreen_draw_mode,
        draw_start_vertex,
        fullscreen_vertex_count
    );
}

} // namespace ltb::app
