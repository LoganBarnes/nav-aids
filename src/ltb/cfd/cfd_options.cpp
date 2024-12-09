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

constexpr auto domain_range_drag_speed = 0.1F;
constexpr auto domain_range_extents    = math::Range< float32 >{ .min = -10.0F, .max = +10.0F };

constexpr auto resolution_drag_speed = 1.0F;
constexpr auto resolution_extents    = math::Range< int32 >{ .min = 1, .max = 526 };

constexpr auto wave_speed_drag_speed = 0.01F;
constexpr auto wave_speed_extents    = math::Range< float32 >{ .min = 0.0F, .max = 1.0F };

constexpr auto time_step_drag_speed = 0.005F;
constexpr auto time_step_extents    = math::Range< float32 >{ .min = 0.0F, .max = 0.1F };

} // namespace

auto configure_gui( CfdOptions< 1 >& options ) -> void
{
    if ( auto const domain_range_min_label = fmt::format(
             "Domain Range Min [{}, {}]",
             domain_range_extents.min,
             domain_range_extents.max
         );
         ImGui::DragFloat(
             domain_range_min_label.c_str( ),
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

    if ( auto const domain_range_max_label = fmt::format(
             "Domain Range Max [{}, {}]",
             domain_range_extents.min,
             domain_range_extents.max
         );
         ImGui::DragFloat(
             domain_range_max_label.c_str( ),
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

    if ( auto const domain_resolution_label = fmt::
             format( "Domain Resolution [{}, {}]", resolution_extents.min, resolution_extents.max );
         ImGui::DragInt(
             domain_resolution_label.c_str( ),
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

    if ( auto const wave_speed_label
         = fmt::format( "Wave Speed [{}, {}]", wave_speed_extents.min, wave_speed_extents.max );
         ImGui::DragFloat(
             wave_speed_label.c_str( ),
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

    if ( auto const time_step_label
         = fmt::format( "Time Step (s) [{}, {}]", time_step_extents.min, time_step_extents.max );
         ImGui::DragFloat(
             time_step_label.c_str( ),
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
}

} // namespace ltb::cfd
