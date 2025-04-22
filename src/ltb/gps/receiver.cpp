#include "ltb/gps/receiver.hpp"

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
namespace
{

constexpr auto receiver_radius = 0.1F;
constexpr auto receiver_color  = glm::vec4{ 1.0F, 0.0F, 0.0F, 1.0F };

constexpr auto max_lat_long_alt = glm::vec3{ +90.0F, +180.0F, 64'000.0F };
constexpr auto min_lat_long_alt = glm::vec3{ -90.0F, -180.0F, 0.0F };

} // namespace

Receiver::Receiver( gui::MeshDisplayPipeline& mesh_pipeline )
    : Receiver( mesh_pipeline, { 0.0F, 0.0F, 0.0F } )
{
}

Receiver::Receiver( gui::MeshDisplayPipeline& mesh_pipeline, glm::vec3 lat_long_alt_ft )
    : mesh_pipeline_( mesh_pipeline )
    , lat_long_alt_ft_( lat_long_alt_ft )
{
}

auto Receiver::initialize( ) -> utils::Result<>
{
    auto const position_mesh = build_mesh(
        math::shapes::Icosphere{
            .position        = { 0.0F, 0.0F, 0.0F },
            .radius          = receiver_radius,
            .recursion_level = 2U,
        }
    );
    LTB_CHECK( reciever_point_id_, mesh_pipeline_.initialize_mesh( position_mesh ) );

    mesh_pipeline_.update_settings(
        reciever_point_id_,
        gui::MeshDisplaySettings{
            .visible      = true,
            .color_mode   = gui::ColorMode::GlobalColor,
            .custom_color = receiver_color,
            .shading_mode = gui::ShadingMode::Flat,
        }
    );

    update_transform( );

    return utils::success( );
}

auto Receiver::is_initialized( ) const -> bool
{
    return !reciever_point_id_.is_nil( );
}

auto Receiver::configure_gui( ) -> void
{
    ImGui::PushID( this );

    if ( ImGui::TreeNodeEx( "Receiver", ImGuiTreeNodeFlags_DefaultOpen ) )
    {
        if ( std::ranges::any_of(
                 std::array{
                     ImGui::DragFloat( "Latitude", &lat_long_alt_ft_.x ),
                     ImGui::DragFloat( "Longitude", &lat_long_alt_ft_.y ),
                     ImGui::DragFloat( "Altitude", &lat_long_alt_ft_.z, 100.0F ),
                 },
                 utils::IsTrue{ }
             ) )
        {
            update_transform( );
        }

        ImGui::TreePop( );
    }

    ImGui::PopID( );
}

auto Receiver::evaluate( std::list< Satellite > const& satellites ) -> utils::Result<>
{
    utils::ignore( satellites );
    return LTB_MAKE_UNEXPECTED_ERROR( "Not implemented yet" );
}

auto Receiver::update_transform( ) -> void
{
    lat_long_alt_ft_ = glm::clamp( lat_long_alt_ft_, min_lat_long_alt, max_lat_long_alt );

    auto const pos = gps::cartesian_from_lat_long_alt_Mm(
        glm::vec3(
            glm::vec2( lat_long_alt_ft_ ),
            Constants< float32 >::Mm_from_ft( lat_long_alt_ft_.z )
        )
    );

    auto settings       = mesh_pipeline_.get_settings( reciever_point_id_ );
    settings.transforms = { math::Translation3d{ pos } };
    mesh_pipeline_.update_settings( reciever_point_id_, settings );
}

} // namespace ltb::gps
