#pragma once

// project
#include "ltb/gps/satellite.hpp"

namespace ltb::gps
{

class Receiver
{
public:
    explicit Receiver( gui::MeshDisplayPipeline& mesh_pipeline );
    explicit Receiver( gui::MeshDisplayPipeline& mesh_pipeline, glm::vec3 lat_long_alt_ft );

    auto initialize( ) -> utils::Result<>;

    [[nodiscard( "Receiver is not initialized if this returns false." )]]
    auto is_initialized( ) const -> bool;

    auto configure_gui( ) -> void;

    auto evaluate( std::list< Satellite > const& satellites ) -> utils::Result<>;

private:
    gui::MeshDisplayPipeline& mesh_pipeline_;
    glm::vec3                 lat_long_alt_ft_;

    gui::MeshId                reciever_point_id_ = gui::MeshId::nil( );
    std::vector< gui::MeshId > range_spheres_     = { };

    auto update_transform( ) -> void;
};

} // namespace ltb::gps
