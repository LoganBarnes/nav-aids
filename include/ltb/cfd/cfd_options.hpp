#pragma once

// project
#include "ltb/math/range.hpp"
#include "ltb/utils/types.hpp"

namespace ltb::cfd
{

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

auto configure_gui( CfdOptions< 1 >& options ) -> void;

} // namespace ltb::cfd
