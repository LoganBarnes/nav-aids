#include "ltb/gps/satellite.hpp"

// project
#include "ltb/gps/constants.hpp"
#include "ltb/gps/utils.hpp"
#include "ltb/gui/imgui.hpp"
#include "ltb/math/shapes/icosphere.hpp"
#include "ltb/utils/container_utils.hpp"

// standard
#include <algorithm>
#include <array>

namespace ltb::gps
{

Satellite::Satellite( gui::MeshDisplayPipeline& mesh_pipeline )
    : mesh_pipeline_( mesh_pipeline )
{
}

auto Satellite::initialize( ) -> utils::Result< void >
{
    auto const satellite_mesh = build_mesh(
        math::shapes::Icosphere{
            .position        = { 0.0F, 0.0F, 0.0F },
            .radius          = 1.0F,
            .recursion_level = 2U,
        }
    );
    LTB_CHECK( satellite_point_id_, mesh_pipeline_.initialize_mesh( satellite_mesh ) );

    mesh_pipeline_.update_settings(
        satellite_point_id_,
        gui::MeshDisplaySettings{
            .visible      = true,
            .color_mode   = gui::ColorMode::GlobalColor,
            .custom_color = { 1.0F, 1.0F, 1.0F, 1.0F },
            .shading_mode = gui::ShadingMode::Flat,
        }
    );

    update_transform( );

    return utils::success( );
}

auto Satellite::is_initialized( ) const -> bool
{
    return !satellite_point_id_.is_nil( );
}

auto Satellite::configure_gui( ) -> void
{
    ImGui::PushID( this );

    if ( ImGui::TreeNode( "Satellite" ) )
    {
        if (auto const values = std::array{
            ImGui::InputFloat( "Latitude", &lat_long_.x, 0.1F, 1.0F, "%.2F" ),
            ImGui::InputFloat( "Longitude", &lat_long_.y, 0.1F, 1.0F, "%.2F" ),
        }; std::ranges::any_of( values, utils::IsTrue{} ))
        {
            update_transform( );
        }

        ImGui::TreePop( );
    }

    ImGui::PopID( );
}

auto Satellite::update_transform( ) -> void
{
    auto const pos = gps::cartesian_from_lat_long_alt_Mm(
        glm::vec3{ lat_long_, gps::Constants< float32 >::satellite_altitude_Mm( ) }
    );

    auto settings       = mesh_pipeline_.get_settings( satellite_point_id_ );
    settings.transforms = { math::Translation3d{ pos } };
    mesh_pipeline_.update_settings( satellite_point_id_, settings );
}

} // namespace ltb::gps
