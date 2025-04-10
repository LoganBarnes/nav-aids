#include "ltb/app/gps_app.hpp"

// project
#include "ltb/math/shapes/icosphere.hpp"

// external
#include <magic_enum.hpp>

namespace ltb::app
{
namespace
{

} // namespace

auto GpsApp::initialize( glm::ivec2 const framebuffer_size ) -> utils::Result< void >
{
    LTB_CHECK( utils::initialize( mesh_pipeline_ ) );

    glClearColor( 0.2F, 0.2F, 0.2F, 1.0F );
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    auto const earth_mesh = build_mesh(
        math::shapes::Icosphere{
            .position        = { 0.0F, 0.0F, 0.0F },
            .radius          = earth_radius_Mm,
            .recursion_level = 4U,
        }
    );
    LTB_CHECK( earth_id_, mesh_pipeline_.initialize_mesh( earth_mesh ) );
    mesh_pipeline_.update_settings(
        earth_id_,
        gui::MeshDisplaySettings{
            .visible      = true,
            .color_mode   = gui::ColorMode::GlobalColor,
            .custom_color = { 0.1F, 0.5F, 0.1F, 1.0F },
            .shading_mode = gui::ShadingMode::Flat,
        }
    );

    // auto settings                 = camera_.settings( );
    // settings.initial_eye_position = { 0.0F, 0.0F, 200.0F };
    // settings.initial_focus_point  = { 0.0F, 0.0F, 0.0F };
    // settings.initial_up_direction = { 0.0F, 1.0F, 0.0F };
    // camera_.set_settings( settings );
    // camera_.reset( );
    // camera_.update();

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
