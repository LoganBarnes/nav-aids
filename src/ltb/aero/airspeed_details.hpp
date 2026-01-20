#pragma once

// project
#include "ltb/aero/airspeed.hpp"
#include "ltb/utils/types.hpp"

namespace ltb::aero::details
{

/// \brief A struct that provides specialized limitation functions
///        specific to the provided aircraft type.
template < AircraftType Aircraft >
class Airspeed
{
public:
    static auto look_up_cas_knots( CasParameters< float64 > const& parameters ) -> float64;
};

} // namespace ltb::aero::details
