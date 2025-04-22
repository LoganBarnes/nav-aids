#include "ltb/app/gps_app.hpp"

// project
#include "ltb/gps/constants.hpp"
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

    auto const axis_mesh = math::Mesh3{
        .format = math::MeshFormat::Lines,
        .positions = {
            { 0.0F, 0.0F, 0.0F },
            { 1.0F, 0.0F, 0.0F },
            { 0.0F, 0.0F, 0.0F },
            { 0.0F, 1.0F, 0.0F },
            { 0.0F, 0.0F, 0.0F },
            { 0.0F, 0.0F, 1.0F },
        },
        .vertex_colors = {
            { 1.0F, 0.0F, 0.0F },
            { 1.0F, 0.0F, 0.0F },
            { 0.0F, 1.0F, 0.0F },
            { 0.0F, 1.0F, 0.0F },
            { 0.0F, 0.0F, 1.0F },
            { 0.0F, 0.0F, 1.0F },
        }
    };
    LTB_CHECK( axis_id_, mesh_pipeline_.initialize_mesh( axis_mesh ) );
    mesh_pipeline_.update_settings(
        axis_id_,
        gui::MeshDisplaySettings{
            .visible      = false,
            .color_mode   = gui::ColorMode::VertexColor,
            .shading_mode = gui::ShadingMode::Flat,
        }
    );

    auto const earth_mesh = build_mesh(
        math::shapes::Icosphere{
            .position        = { 0.0F, 0.0F, 0.0F },
            .radius          = gps::Constants< float32 >::earth_radius_Mm( ),
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

    LTB_CHECK(
        utils::initialize(
            satellites_.emplace_back( mesh_pipeline_, glm::vec2{ 55.0F, 0.0F } ),
            satellites_.emplace_back( mesh_pipeline_, glm::vec2{ 90.0F, 0.0F } ),
            satellites_.emplace_back( mesh_pipeline_, glm::vec2{ 55.0F, 180.0F } ),
            receivers_.emplace_back( mesh_pipeline_, glm::vec3{ 90.0F, 0.0F, 10'000.0F } )
        )
    );

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

    if ( ImGui::Begin( "Satellites" ) )
    {
        for ( auto& satellite : satellites_ )
        {
            satellite.configure_gui( );
        }

        if ( ImGui::Button( "Add Satellite" ) )
        {
            auto satellite = gps::Satellite{ mesh_pipeline_ };
            if ( auto result = satellite.initialize( ) )
            {
                satellites_.push_back( std::move( satellite ) );
            }
            else
            {
                error_alert_.to_display( result.error( ) );
            }
        }
    }
    ImGui::End( );

    if ( ImGui::Begin( "Receiver" ) )
    {

        for ( auto& receiver : receivers_ )
        {
            receiver.configure_gui( );
        }

        if ( ImGui::Button( "Add Receiver" ) )
        {
            auto receiver = gps::Receiver{ mesh_pipeline_ };
            if ( auto result = receiver.initialize( ) )
            {
                receivers_.push_back( std::move( receiver ) );
            }
            else
            {
                error_alert_.to_display( result.error( ) );
            }
        }
    }
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
