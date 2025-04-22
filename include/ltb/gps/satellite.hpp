#pragma once

// project
#include "ltb/gui/mesh_display_pipeline.hpp"

namespace ltb::gps
{

class Satellite
{
public:
    explicit Satellite( gui::MeshDisplayPipeline& mesh_pipeline );

    auto initialize( ) -> utils::Result< void >;
    auto is_initialized( ) const -> bool;

    auto configure_gui( ) -> void;

private:
    gui::MeshDisplayPipeline& mesh_pipeline_;
    glm::vec2                 lat_long_ = { 0.0F, 0.0F };

    gui::MeshId satellite_point_id_ = gui::MeshId::nil( );

    auto update_transform() -> void;
};

} // namespace ltb::gps
