#include "ltb/app/ils_app.hpp"

// project
#include "ltb/utils/error_callback.hpp"

// generated
#include "ltb/ltb_config.hpp"

// external
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>

// ILS Interference Graphics
// https://www.desmos.com/calculator/l0sj535wrs

namespace ltb::app
{
namespace
{

// Speed of light in m/µs
constexpr auto c = 299.792458;

constexpr auto white  = ImColor( ImVec4( 1.0F, 1.0F, 1.0F, 1.0F ) );
constexpr auto orange = ImColor( ImVec4( 1.0F, 0.5F, 0.1F, 1.0F ) );

constexpr auto radio_frequency_mhz = 110.1;

auto draw_phase_circle( glm::dvec2 const wave )
{
    auto* const draw_list = ImGui::GetWindowDrawList( );

    auto const content_size = glm::vec2{ ImGui::GetContentRegionAvail( ) };
    auto const size         = std::min( content_size.x, content_size.y );
    auto const radius       = size * 0.5F;

    auto const cursor = glm::vec2{ ImGui::GetCursorScreenPos( ) };

    auto const center = cursor + radius;

    draw_list->AddCircle( center, radius, white );
    draw_list
        ->AddLine( cursor + glm::vec2{ 0.0F, radius }, cursor + glm::vec2{ size, radius }, white );
    draw_list
        ->AddLine( cursor + glm::vec2{ radius, 0.0F }, cursor + glm::vec2{ radius, size }, white );

    auto const scaled_wave = glm::vec2( wave ) * radius * 0.5F;

    draw_list->AddLine( center, center + scaled_wave, orange, 5.0F );

    draw_list->AddLine(
        cursor + size + glm::vec2( 20.0F, 0.0F ),
        cursor + size + glm::vec2{ 20.0F, -glm::length( scaled_wave ) },
        orange,
        10.0F
    );
}

auto draw_phase(
    glm::dvec2 const world_pos,
    glm::dvec2 const antenna_pos,
    float64 const    scale,
    float64 const    shift
)
{
    auto const distance_meters = glm::distance( world_pos, antenna_pos );
    ImGui::Text( "Antenna 1: %.2F m", distance_meters );
    auto const microseconds = distance_meters / c;
    ImGui::Text( "Time delay: %.9F µs", microseconds );
    auto const phase_angle = radio_frequency_mhz * microseconds;
    ImGui::Text( "Phase angle: %.2F", phase_angle );

    auto const radians = ( phase_angle * glm::two_pi< float64 >( ) ) + shift;

    auto const wave = glm::dvec2{ std::cos( radians ), std::sin( radians ) } * scale;

    draw_phase_circle( wave );

    return wave;
}

} // namespace

auto IlsApp::initialize( glm::ivec2 const framebuffer_size ) -> utils::Result< void >
{
    framebuffer_size_ = framebuffer_size;

    LTB_CHECK(
        utils::initialize(
            fullscreen_vertex_shader_,
            ils_fragment_shader_,
            program_,
            antenna_pairs_uniform_,
            antenna_spacing_m_uniform_,
            output_scale_uniform_,
            // time_s_uniform_,
            world_from_clip_uniform_,
            vertex_array_
        )
    );

    glClearColor( 0.0F, 1.0F, 0.0F, 1.0F );
    glDisable( GL_DEPTH_TEST );

    start_time_ = std::chrono::steady_clock::now( );

    camera_.set_width( 50.0F );
    camera_.set_center( { 0.0F, 0.0F } );

    return utils::success( );
}

auto IlsApp::render( ) -> void
{
    glViewport( 0, 0, framebuffer_size_.x, framebuffer_size_.y );
    glClear( GL_COLOR_BUFFER_BIT );

    auto const current_time     = std::chrono::steady_clock::now( );
    auto const elapsed_duration = current_time - start_time_;
    auto const elapsed_time_s
        = std::chrono::duration_cast< std::chrono::duration< float32 > >( elapsed_duration )
              .count( );

    auto const cam_uniforms = camera_.simple_render_params( );

    // set( pixel_size_m_uniform_, pixel_size_m_ );
    set( antenna_pairs_uniform_, antenna_pairs_ );
    set( antenna_spacing_m_uniform_, antenna_spacing_m_ );
    set( output_scale_uniform_, output_channels_ * output_scale_ );
    set( time_s_uniform_, elapsed_time_s * time_scale_s_ );
    // set( field_size_pixels_uniform_, glm::vec2{ framebuffer_size_ } );
    set( world_from_clip_uniform_, cam_uniforms.world_from_clip );

    ogl::draw( ogl::bind( program_ ), ogl::bind( vertex_array_ ), GL_TRIANGLE_STRIP, 0, 4 );
}

auto IlsApp::configure_gui( ) -> void
{
    utils::ignore( camera_.handle_inputs( ) );

    constexpr auto dock_node_flags = ImGuiDockNodeFlags_PassthruCentralNode;
    utils::ignore( ImGui::DockSpaceOverViewport( 0, nullptr, dock_node_flags ) );

    if ( ImGui::Begin( "ILS" ) )
    {
        auto const unused_return_values = std::array{
            // ImGui::SliderFloat( "Pixel size (m)", &pixel_size_m_, 0.1F, 10.0F ),
            ImGui::SliderInt( "Antenna pairs", &antenna_pairs_, 1, 10 ),
            ImGui::SliderFloat( "Antenna spacing (m)", &antenna_spacing_m_, 0.1F, 10.0F ),
        };
        utils::ignore( unused_return_values );

        output_scale_ = 0.1F / static_cast< float32 >( antenna_pairs_ );

        auto const* str = "Both";
        switch ( display_ )
        {
            using enum Display;
            case CSB:
                str = "CSB";
                break;
            case SBO:
                str = "SBO";
                break;
            case Both:
                break;
        }

        if ( ImGui::BeginCombo( "Pattern", str ) )
        {
            using enum Display;
            if ( ImGui::Selectable( "CSB" ) )
            {
                output_channels_ = { 0.0F, 1.0F, 0.0F };
                display_         = CSB;
            }
            if ( ImGui::Selectable( "SBO" ) )
            {
                output_channels_ = { 0.0F, 0.0F, 1.0F };
                display_         = SBO;
            }
            if ( ImGui::Selectable( "Both" ) )
            {
                output_channels_ = { 0.0F, 1.0F, 1.0F };
                display_         = Both;
            }
            ImGui::EndCombo( );
        }
    }
    ImGui::End( );

    auto const antenna_spacing_m = static_cast< float64 >( antenna_spacing_m_ );
    auto const cam               = camera_.simple_render_params( );

    auto const mouse_pos = glm::vec2{ ImGui::GetMousePos( ) };
    auto const clip_pos  = ( ( mouse_pos / glm::vec2{ framebuffer_size_ } ) * 2.0F ) - 1.0F;
    auto const world_pos
        = glm::dvec2( cam.world_from_clip * glm::vec4( clip_pos.x, -clip_pos.y, 0.0F, 1.0F ) );

    ImGui::SetTooltip( "World pos: (%.2F, %.2F) m", world_pos.x, world_pos.y );

    auto const antenna_1 = glm::dvec2{ 0.0, +antenna_spacing_m * 0.5 };
    auto const antenna_2 = glm::dvec2{ 0.0, -antenna_spacing_m * 0.5 };

    auto wave_1 = glm::dvec2{ 0.0, 0.0 };
    auto wave_2 = glm::dvec2{ 0.0, 0.0 };

    if ( ImGui::Begin( "CSB 1" ) )
    {
        wave_1 = draw_phase( world_pos, antenna_1, 1.0, 0.0 );
    }
    ImGui::End( );

    if ( ImGui::Begin( "CSB 2" ) )
    {
        wave_2 = draw_phase( world_pos, antenna_2, 1.0, 0.0 );
    }
    ImGui::End( );

    if ( ImGui::Begin( "CSB" ) )
    {
        draw_phase_circle( wave_1 + wave_2 );
    }
    ImGui::End( );

    if ( ImGui::Begin( "SBO 1" ) )
    {
        wave_1 = draw_phase( world_pos, antenna_1, -1.0, glm::half_pi< float64 >( ) );
    }
    ImGui::End( );

    if ( ImGui::Begin( "SBO 2" ) )
    {
        wave_2 = draw_phase( world_pos, antenna_2, +1.0, glm::half_pi< float64 >( ) );
    }
    ImGui::End( );

    if ( ImGui::Begin( "SBO" ) )
    {
        draw_phase_circle( wave_1 + wave_2 );
    }
    ImGui::End( );

    constexpr auto radio_frequency        = 30.0;
    constexpr auto ninety_hz_frequency    = 0.9;
    constexpr auto one_fifty_hz_frequency = 1.5;

    constexpr auto sample_points_i = 8192;
    constexpr auto sample_points   = static_cast< uint64 >( sample_points_i );
    constexpr auto window_range_us = math::Range{ .min = 0.0, .max = 2.0 };
    constexpr auto window_size_us  = dimensions( window_range_us );

    auto carrier_wave = std::vector( sample_points, 0.0F );
    assert( carrier_wave.size( ) == sample_points );
    auto ninety_hz_wave = std::vector( sample_points, 0.0F );
    assert( ninety_hz_wave.size( ) == sample_points );
    auto one_fifty_hz_wave = std::vector( sample_points, 0.0F );
    assert( one_fifty_hz_wave.size( ) == sample_points );
    auto csb_audio_wave = std::vector( sample_points, 0.0F );
    assert( csb_audio_wave.size( ) == sample_points );
    auto csb_modulated_wave = std::vector( sample_points, 0.0F );
    assert( csb_modulated_wave.size( ) == sample_points );
    auto csb_signal_wave = std::vector( sample_points, 0.0F );
    assert( csb_signal_wave.size( ) == sample_points );
    auto sbo_audio_wave = std::vector( sample_points, 0.0F );
    assert( sbo_audio_wave.size( ) == sample_points );
    auto sbo_modulated_wave = std::vector( sample_points, 0.0F );
    assert( sbo_modulated_wave.size( ) == sample_points );
    auto sbo_shifted_wave = std::vector( sample_points, 0.0F );
    assert( sbo_shifted_wave.size( ) == sample_points );

    for ( auto i = 0UZ; i < sample_points; ++i )
    {
        auto const t = static_cast< float64 >( i ) / static_cast< float64 >( sample_points );

        auto const us = window_range_us.min + ( t * window_size_us );

        carrier_wave[ i ] = static_cast< float32 >(
            std::sin( glm::two_pi< float64 >( ) * radio_frequency * us )
        );
        ninety_hz_wave[ i ] = static_cast< float32 >(
            std::sin( glm::two_pi< float64 >( ) * ninety_hz_frequency * us )
        );
        one_fifty_hz_wave[ i ] = static_cast< float32 >(
            std::sin( glm::two_pi< float64 >( ) * one_fifty_hz_frequency * us )
        );

        csb_audio_wave[ i ]     = ninety_hz_wave[ i ] + one_fifty_hz_wave[ i ];
        csb_modulated_wave[ i ] = carrier_wave[ i ] * csb_audio_wave[ i ] * 0.2F;
        csb_signal_wave[ i ]    = csb_modulated_wave[ i ] + carrier_wave[ i ];

        auto const shifted_carrier_wave = static_cast< float32 >( std::sin(
            glm::half_pi< float64 >( ) + ( glm::two_pi< float64 >( ) * radio_frequency * us )
        ) );

        sbo_audio_wave[ i ]     = ninety_hz_wave[ i ] - one_fifty_hz_wave[ i ];
        sbo_modulated_wave[ i ] = carrier_wave[ i ] * sbo_audio_wave[ i ] * 0.2F;
        sbo_shifted_wave[ i ]   = shifted_carrier_wave * sbo_audio_wave[ i ] * 0.2F;
    }

    if ( ImGui::Begin( "Transmitted Waves" ) )
    {
        ImGui::PlotLines(
            "Radio Carrier",
            carrier_wave.data( ),
            sample_points_i,
            0,
            nullptr,
            -2.0F,
            2.0F,
            ImVec2( 0.0F, 100.0F )
        );
        ImGui::PlotLines(
            "90Hz Audio",
            ninety_hz_wave.data( ),
            sample_points_i,
            0,
            nullptr,
            -2.0F,
            2.0F,
            ImVec2( 0.0F, 100.0F )
        );
        ImGui::PlotLines(
            "150Hz Audio",
            one_fifty_hz_wave.data( ),
            sample_points_i,
            0,
            nullptr,
            -2.0F,
            2.0F,
            ImVec2( 0.0F, 100.0F )
        );
        ImGui::PlotLines(
            "CSB Audio (90Hz + 150Hz)",
            csb_audio_wave.data( ),
            sample_points_i,
            0,
            nullptr,
            -2.0F,
            2.0F,
            ImVec2( 0.0F, 100.0F )
        );
        ImGui::PlotLines(
            "CSB Modulated",
            csb_modulated_wave.data( ),
            sample_points_i,
            0,
            nullptr,
            -2.0F,
            2.0F,
            ImVec2( 0.0F, 100.0F )
        );
        ImGui::PlotLines(
            "CSB Signal",
            csb_signal_wave.data( ),
            sample_points_i,
            0,
            nullptr,
            -2.0F,
            2.0F,
            ImVec2( 0.0F, 100.0F )
        );
        ImGui::PlotLines(
            "SBO Audio (90Hz - 150Hz)",
            sbo_audio_wave.data( ),
            sample_points_i,
            0,
            nullptr,
            -2.0F,
            2.0F,
            ImVec2( 0.0F, 100.0F )
        );
        ImGui::PlotLines(
            "SBO Modulated",
            sbo_modulated_wave.data( ),
            sample_points_i,
            0,
            nullptr,
            -2.0F,
            2.0F,
            ImVec2( 0.0F, 100.0F )
        );
        ImGui::PlotLines(
            "SBO Shifted",
            sbo_shifted_wave.data( ),
            sample_points_i,
            0,
            nullptr,
            -2.0F,
            2.0F,
            ImVec2( 0.0F, 100.0F )
        );
    }
    ImGui::End( );

    // auto const radians = phase_angle * glm::two_pi< float64 >( );

    // auto const wave = glm::dvec2{ std::cos( radians ), std::sin( radians ) } * scale;
}

auto IlsApp::destroy( ) -> void
{
    vertex_array_ = { };

    program_ = { fullscreen_vertex_shader_, ils_fragment_shader_ };

    framebuffer_size_ = { };
}

auto IlsApp::resize( glm::ivec2 const framebuffer_size ) -> void
{
    framebuffer_size_ = framebuffer_size;

    camera_.resize( glm::vec2( framebuffer_size_ ) );
}
} // namespace ltb::app
