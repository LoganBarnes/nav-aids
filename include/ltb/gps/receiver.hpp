#pragma once

// project
#include "ltb/gui/mesh_display_pipeline.hpp"

namespace ltb::gps
{

class Receiver
{
public:
    Receiver( )  = default;
    ~Receiver( ) = default;

    auto configure_gui( ) -> void;

private:
    glm::vec3 lat_long_alt_Mm_ = { 0.0F, 0.0F, 0.0F };

    gui::MeshId position_id_ = gui::MeshId::nil( );

    std::vector< gui::MeshId > range_spheres_ = { };
};

} // namespace ltb::gps
