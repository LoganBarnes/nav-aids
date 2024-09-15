#include "ltb/cfd/cfd_options.hpp"

// project
#include "ltb/gui/imgui.hpp"

// external
#include <spdlog/fmt/fmt.h>

// standard
#include <algorithm>

namespace ltb::cfd
{
namespace
{

auto tooltip( std::string const& text )
{
    if ( ImGui::IsItemHovered( ) )
    {
        ImGui::SetTooltip( "%s", text.c_str( ) );
    }
}

} // namespace

auto domain_step( CfdOptions< 1 >& options ) -> float32
{
    return math::dimensions( options.domain_range )
         / static_cast< float32 >( options.domain_resolution );
}

auto configure_gui( CfdOptions< 1 >& options ) -> void
{
    if ( ImGui::DragFloat(
             "Domain Range Min",
             &options.domain_range.min,
             domain_range_drag_speed,
             domain_range_extents.min,
             options.domain_range.max
         ) )
    {
        options.domain_range.min = std::clamp(
            // clamp between min value and existing max.
            options.domain_range.min,
            domain_range_extents.min,
            options.domain_range.max
        );
    }
    tooltip( fmt::format( "[{}, {}]", domain_range_extents.min, domain_range_extents.max ) );

    if ( ImGui::DragFloat(
             "Domain Range Max",
             &options.domain_range.max,
             domain_range_drag_speed,
             options.domain_range.min,
             domain_range_extents.max
         ) )
    {
        options.domain_range.max = std::clamp(
            // clamp between existing min and max value.
            options.domain_range.max,
            options.domain_range.min,
            domain_range_extents.max
        );
    }
    tooltip( fmt::format( "[{}, {}]", domain_range_extents.min, domain_range_extents.max ) );

    if ( ImGui::DragInt(
             "Domain Resolution",
             &options.domain_resolution,
             resolution_drag_speed,
             resolution_extents.min,
             resolution_extents.max
         ) )
    {
        options.domain_resolution = std::clamp(
            // Clamp between min and max.
            options.domain_resolution,
            resolution_extents.min,
            resolution_extents.max
        );
    }
    tooltip( fmt::format( "[{}, {}]", resolution_extents.min, resolution_extents.max ) );

    ImGui::Text( "Domain Step: %.4f", domain_step( options ) );

    if ( ImGui::DragFloat(
             "Wave Speed",
             &options.wave_speed,
             wave_speed_drag_speed,
             wave_speed_extents.min,
             wave_speed_extents.max
         ) )
    {
        options.wave_speed = std::clamp(
            // Clamp between min and max.
            options.wave_speed,
            wave_speed_extents.min,
            wave_speed_extents.max
        );
    }
    tooltip( fmt::format( "[{}, {}]", wave_speed_extents.min, wave_speed_extents.max ) );

    if ( ImGui::DragFloat(
             "Time Step (s) ",
             &options.time_step_s,
             time_step_drag_speed,
             time_step_extents.min,
             time_step_extents.max
         ) )
    {
        options.time_step_s = std::clamp(
            // Clamp between min and max.
            options.time_step_s,
            time_step_extents.min,
            time_step_extents.max
        );
    }
    tooltip( fmt::format( "[{}, {}]", time_step_extents.min, time_step_extents.max ) );
}

} // namespace ltb::cfd
