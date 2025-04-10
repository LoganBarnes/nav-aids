#pragma once

// project
#include "ltb/app/app.hpp"
// #include "ltb/gui/cam/orbit_camera.hpp"
#include "ltb/gui/cam/arcball_camera.hpp"
#include "ltb/gui/incremental_id_generator.hpp"
#include "ltb/gui/mesh_display_pipeline.hpp"
#include "ltb/ogl/framebuffer.hpp"
#include "ltb/utils/initializable.hpp"

// generated
#include "ltb/ltb_config.hpp"

// standard
#include <chrono>

namespace ltb::app
{

constexpr auto gps_satellite_altitude_Mm = 20.2F;
constexpr auto earth_radius_Mm           = 6.378F;

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
    gui::cam::ArcballCamera       camera_                = { };
    // gui::cam::OrbitCamera         camera_                = { };

    gui::MeshId earth_id_ = gui::MeshId::nil( );

    struct Satellite
    {
        glm::vec3 lat_long_alt_km = { 0.0F, 0.0F, gps_satellite_altitude_Mm };

        gui::MeshId satellite_point_id = gui::MeshId::nil( );
        gui::MeshId range_sphere_id    = gui::MeshId::nil( );
    };

    std::vector< Satellite > satellites_ = { };

    struct Receiver
    {
        glm::vec3 lat_long_alt_km = { 0.0F, 0.0F, 0.0F };

        gui::MeshId receiver_point_id = gui::MeshId::nil( );
    };
};

} // namespace ltb::app
