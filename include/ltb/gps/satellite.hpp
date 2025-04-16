#pragma once

// project
#include "ltb/gps/constants.hpp"
#include "ltb/gui/mesh_display_pipeline.hpp"

namespace ltb::gps
{

class Satellite
{
public:
    Satellite( )  = default;
    ~Satellite( ) = default;

    auto configure_gui( ) -> void;

private:
    glm::vec3 lat_long_alt_Mm_ = { 0.0F, 0.0F, Constants< float32 >::satellite_altitude_Mm( ) };

    gui::MeshId satellite_point_id_ = gui::MeshId::nil( );
    gui::MeshId range_sphere_id_    = gui::MeshId::nil( );
};

} // namespace ltb::gps
