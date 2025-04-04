#include "ltb/app/gps_app.hpp"

// project
#include "ltb/math/shapes/icosphere.hpp"
#include "ltb/utils/error_callback.hpp"

// external
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>

namespace ltb::app
{
namespace
{

} // namespace

auto GpsApp::initialize( glm::ivec2 const framebuffer_size ) -> utils::Result< void >
{
    LTB_CHECK( utils::initialize( mesh_pipeline_ ) );

    glClearColor( 0.0F, 0.0F, 0.0F, 1.0F );
    glDisable( GL_DEPTH_TEST );

    auto const earth_mesh = build_mesh(
        math::shapes::Icosphere{
            .position        = { 0.0F, 0.0F, 0.0F },
            .radius          = earth_radius_km,
            .recursion_level = 2U,
        }
    );
    LTB_CHECK( earth_id_, mesh_pipeline_.initialize_mesh( earth_mesh ) );

    auto cam_settings                 = camera_.settings( );
    cam_settings.initial_eye_position = { 0.0F, 0.0F, 100.0F };
    camera_.set_settings( cam_settings );

    resize( framebuffer_size );

    return utils::success( );
}

auto GpsApp::render( ) -> void
{
    glViewport( 0, 0, framebuffer_size_.x, framebuffer_size_.y );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    mesh_pipeline_.draw( camera_.render_params( ) );
}

auto GpsApp::configure_gui( ) -> void
{
    camera_.handle_inputs( camera_input_settings_ );

    auto const dock_node_flags = ImGuiDockNodeFlags_PassthruCentralNode;
    utils::ignore( ImGui::DockSpaceOverViewport( 0, nullptr, dock_node_flags ) );

    if ( ImGui::Begin( "GPS" ) ) {}
    ImGui::End( );
}

auto GpsApp::destroy( ) -> void
{
    mesh_pipeline_.clear( );
}

auto GpsApp::resize( glm::ivec2 const framebuffer_size ) -> void
{
    framebuffer_size_ = framebuffer_size;
    camera_.set_viewport_size( glm::vec2( framebuffer_size_ ) );
}

} // namespace ltb::app
