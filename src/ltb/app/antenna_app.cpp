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

constexpr auto color_texture_internal_format = GL_RGBA;
constexpr auto color_texture_format          = GL_RGBA;
constexpr auto color_texture_type            = GL_UNSIGNED_BYTE;

} // namespace

auto AntennaApp::initialize( glm::ivec2 const framebuffer_size ) -> utils::Result< void >
{
    auto const shader_dir = config::shader_dir_path( );

    LTB_CHECK( wave_vertex_shader_.initialize( ) );
    LTB_CHECK( wave_fragment_shader_.initialize( ) );
    LTB_CHECK( wave_program_.initialize( ) );
    LTB_CHECK( wave_vertex_shader_.load_and_compile( shader_dir / "wave.vert" ) );
    LTB_CHECK( wave_fragment_shader_.load_and_compile( shader_dir / "wave.frag" ) );
    LTB_CHECK( wave_program_.attach_and_link( wave_vertex_shader_, wave_fragment_shader_ ) );

    LTB_CHECK( antenna_vertex_shader_.initialize( ) );
    LTB_CHECK( antenna_fragment_shader_.initialize( ) );
    LTB_CHECK( antenna_program_.initialize( ) );

    LTB_CHECK( antenna_vertex_shader_.load_and_compile( shader_dir / "antenna.vert" ) );
    LTB_CHECK( antenna_fragment_shader_.load_and_compile( shader_dir / "antenna.frag" ) );
    LTB_CHECK( antenna_program_.attach_and_link( antenna_vertex_shader_, antenna_fragment_shader_ )
    );
    LTB_CHECK( projection_from_world_uniform_.initialize( "projection_from_world" ) );
    LTB_CHECK( time_uniform_.initialize( "time" ) );

    antennas_ = std::vector{
        Antenna{ .world_position = { 0.0F, -1.0F }, .antenna_power = 1.0F },
        Antenna{ .world_position = { 0.0F, +1.0F }, .antenna_power = 1.0F },
    };

    antenna_vertex_buffer_.initialize( );
    antenna_vertex_array_.initialize( );

    // Store the vertex data in a GPU buffer.
    ogl::buffer_data(
        ogl::bind< GL_ARRAY_BUFFER >( antenna_vertex_buffer_ ),
        antennas_,
        GL_STATIC_DRAW
    );

    LTB_CHECK(
        auto const world_position_attrib,
        antenna_program_.get_attribute_location( "world_position" )
    );
    LTB_CHECK(
        auto const antenna_power_attrib,
        antenna_program_.get_attribute_location( "antenna_power" )
    );

    constexpr Antenna const* const null_antenna_ptr = nullptr;
    // Tightly packed.
    constexpr auto total_vertex_stride = 0U;
    // Not instanced
    constexpr auto attrib_divisor = 0U;

    ogl::set_attributes< void >(
        ogl::bind( antenna_vertex_array_ ),
        ogl::bind< GL_ARRAY_BUFFER >( antenna_vertex_buffer_ ),
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

    for ( auto i = 0UZ; i < framebuffer_count_; ++i )
    {
        wave_field_textures_[ i ].initialize( );
        wave_field_framebuffers_[ i ].initialize( );

        // Attach the color texture to the framebuffer.
        constexpr auto null_depth_texture = std::nullopt;
        framebuffer_texture_2d< GL_TEXTURE_2D >(
            bind< GL_FRAMEBUFFER >( wave_field_framebuffers_[ i ] ),
            { wave_field_textures_[ i ] },
            // No depth texture needed for 2D rendering.
            null_depth_texture
        );

        // Specify the color texture parameters.
        auto const bound_texture = bind< GL_TEXTURE_2D >( wave_field_textures_[ i ] );
        tex_parameteri( bound_texture, ogl::TexParams::filter( ), GL_LINEAR );
        tex_parameteri( bound_texture, ogl::TexParams::wrap( ), GL_CLAMP_TO_EDGE );
    }

    resize( framebuffer_size );

    glClearColor( 0.5F, 0.5F, 0.5F, 1.0F );
    return utils::success( );
}

auto AntennaApp::render( ) -> void
{
    auto const viewport_size = ImGui::GetMainViewport( )->Size;

    glViewport(
        0,
        0,
        static_cast< int32 >( viewport_size.x ),
        static_cast< int32 >( viewport_size.y )
    );
    glClear( GL_COLOR_BUFFER_BIT );

    LTB_CHECK_OR( draw( ), utils::log_error );
}

auto AntennaApp::configure_gui( ) -> void {}

auto AntennaApp::destroy( ) -> void
{
    wave_field_framebuffers_ = { };
    wave_field_textures_     = { };

    antenna_vertex_array_  = { };
    antenna_vertex_buffer_ = { };
    wave_program_          = { };
    wave_fragment_shader_  = { };
    wave_vertex_shader_    = { };
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
            color_texture_internal_format,
            color_texture_format,
            color_texture_type,
            mipmap_level
        );

        LTB_CHECK_OR(
            check_framebuffer_status( bind< GL_FRAMEBUFFER >( wave_field_framebuffers_[ i ] ) ),
            utils::log_error
        );
    }
}

auto AntennaApp::draw( ) -> utils::Result< void >
{
    auto constexpr proj_from_world = glm::identity< glm::mat4 >( );

    // Render the wave field.
    ogl::set( projection_from_world_uniform_, proj_from_world );
    ogl::set( time_uniform_, 0.0F );

    auto const bound_program = ogl::bind( antenna_program_ );
    ogl::draw( bound_program, bind( antenna_vertex_array_ ), GL_LINES, 0, 2 );

    return utils::success( );
}

} // namespace ltb::app
