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

} // namespace

auto IlsApp::initialize( glm::ivec2 const framebuffer_size ) -> utils::Result< void >
{
    framebuffer_size_ = framebuffer_size;

    LTB_CHECK(
        utils::initialize(
            fullscreen_vertex_shader_,
            ils_fragment_shader_,
            program_,
            pixel_size_m_uniform_,
            antenna_pairs_uniform_,
            antenna_spacing_m_uniform_,
            output_scale_uniform_,
            // time_s_uniform_,
            field_size_pixels_uniform_,
            vertex_array_
        )
    );

    glClearColor( 0.0F, 1.0F, 0.0F, 1.0F );
    glDisable( GL_DEPTH_TEST );

    start_time_ = std::chrono::steady_clock::now( );

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

    set( pixel_size_m_uniform_, pixel_size_m );
    set( antenna_pairs_uniform_, antenna_pairs_ );
    set( antenna_spacing_m_uniform_, antenna_spacing_m );
    set( output_scale_uniform_, output_channels_ * output_scale_ );
    set( time_s_uniform_, elapsed_time_s * time_scale_s_ );
    set( field_size_pixels_uniform_, glm::vec2{ framebuffer_size_ } );

    ogl::draw( ogl::bind( program_ ), ogl::bind( vertex_array_ ), GL_TRIANGLE_STRIP, 0, 4 );
}

auto IlsApp::configure_gui( ) -> void
{
    auto const dock_node_flags = ImGuiDockNodeFlags_PassthruCentralNode;
    utils::ignore( ImGui::DockSpaceOverViewport( 0, nullptr, dock_node_flags ) );

    if ( ImGui::Begin( "ILS" ) )
    {
        auto const unused_return_values = std::array{
            ImGui::SliderFloat( "Pixel size (m)", &pixel_size_m, 0.1F, 10.0F ),
            ImGui::SliderInt( "Antenna pairs", &antenna_pairs_, 1, 10 ),
            ImGui::SliderFloat( "Antenna spacing (m)", &antenna_spacing_m, 0.1F, 10.0F ),
        };
        utils::ignore( unused_return_values );

        output_scale_ = 0.1F / static_cast< float32 >( antenna_pairs_ );

        auto* str = "";
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
                str = "Both";
                break;
        }

        if ( ImGui::BeginCombo( "Pattern", str ) )
        {
            if ( ImGui::Selectable( "CSB" ) )
            {
                output_channels_ = { 0.0F, 1.0F, 0.0F };
                display_         = Display::CSB;
            }
            if ( ImGui::Selectable( "SBO" ) )
            {
                output_channels_ = { 0.0F, 0.0F, 1.0F };
                display_         = Display::SBO;
            }
            if ( ImGui::Selectable( "Both" ) )
            {
                output_channels_ = { 0.0F, 1.0F, 1.0F };
                display_         = Display::Both;
            }
            ImGui::EndCombo( );
        }
    }
    ImGui::End( );
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
}

} // namespace ltb::app
