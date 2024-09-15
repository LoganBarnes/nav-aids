#pragma once

// project
#include "ltb/math/range.hpp"
#include "ltb/utils/types.hpp"

namespace ltb::cfd
{

constexpr auto domain_range_drag_speed = 0.1F;
constexpr auto domain_range_extents    = math::Range< float32 >{ .min = -10.0F, .max = +10.0F };

constexpr auto resolution_drag_speed = 1.0F;
constexpr auto resolution_extents    = math::Range< int32 >{ .min = 1, .max = 512 };

constexpr auto wave_speed_drag_speed = 0.01F;
constexpr auto wave_speed_extents    = math::Range< float32 >{ .min = 0.0F, .max = 1.0F };

constexpr auto time_step_drag_speed = 0.005F;
constexpr auto time_step_extents    = math::Range< float32 >{ .min = 0.0F, .max = 0.1F };

template < glm::length_t Dimensions >
struct CfdOptions;

template <>
struct CfdOptions< 1 >
{
    math::Range< float32 > domain_range      = { .min = -1.0F, .max = 1.0F };
    int32                  domain_resolution = 64;
    float32                wave_speed        = 1.0F;
    float32                time_step_s       = 0.025F;
};

auto domain_step( CfdOptions< 1 >& options ) -> float32;

auto configure_gui( CfdOptions< 1 >& options ) -> void;

} // namespace ltb::cfd
