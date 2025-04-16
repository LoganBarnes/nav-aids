#pragma once

// project
#include "ltb/app/app.hpp"
#include "ltb/gps/receiver.hpp"
#include "ltb/gps/satellite.hpp"
#include "ltb/gui/cam/arcball_camera.hpp"
#include "ltb/gui/cam/orbit_camera.hpp"
#include "ltb/gui/incremental_id_generator.hpp"
#include "ltb/gui/mesh_display_pipeline.hpp"
#include "ltb/utils/initializable.hpp"

// standard
#include <chrono>

namespace ltb::app
{

/// \brief The main application window for the ILS app.
class GpsApp : public App
{
public:
    GpsApp( )           = default;
    ~GpsApp( ) override = default;

    auto initialize( glm::ivec2 framebuffer_size ) -> utils::Result< void > override;
    auto render( ) -> void override;
    auto configure_gui( ) -> void override;
    auto destroy( ) -> void override;

    auto resize( glm::ivec2 framebuffer_size ) -> void override;

private:
    gui::IncrementalIdGenerator< uint32 > id_generator_  = { };
    gui::MeshDisplayPipeline              mesh_pipeline_ = { id_generator_ };

    glm::ivec2 framebuffer_size_ = { };

    math::cam::CameraSettings     camera_settings_       = { };
    gui::cam::CameraInputSettings camera_input_settings_ = { };
    gui::cam::OrbitCamera         camera_                = { };

    gui::MeshId earth_id_ = gui::MeshId::nil( );

    std::vector< gps::Satellite > satellites_ = { };
    std::vector< gps::Receiver >  receivers_  = { };
};

} // namespace ltb::app
