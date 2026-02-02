#include "ltb/aero/airspeed.hpp"

// project
#include "ltb/aero/airspeed_details.hpp"
#include "ltb/utils/types.hpp"

namespace ltb::aero
{

template <>
auto look_up_cas_knots( AircraftType const aircraft, CasParameters< float64 > const& params )
    -> float64
{
    switch ( aircraft )
    {
        using enum AircraftType;
        case RobinsonR22BetaII:
            return details::Airspeed< RobinsonR22BetaII >::look_up_cas_knots( params );
        case RobinsonR44II:
            return details::Airspeed< RobinsonR44II >::look_up_cas_knots( params );

            // Currently assuming no calibrated airspeed tables for these aircraft.
        case Unknown:
            break;
    }

    return params.indicated_airspeed_knots;
}

// Float specialization
template <>
auto look_up_cas_knots( AircraftType const aircraft, CasParameters< float32 > const& params )
    -> float32
{
    auto const double_params = CasParameters< float64 >{
        .indicated_airspeed_knots = static_cast< float64 >( params.indicated_airspeed_knots ),
    };
    auto const double_cas_knots = look_up_cas_knots< float64 >( aircraft, double_params );
    return static_cast< float32 >( double_cas_knots );
}

} // namespace ltb::aero
