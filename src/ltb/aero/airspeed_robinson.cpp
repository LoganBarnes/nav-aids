#include "ltb/aero/airspeed_details.hpp"

// project
#include "ltb/math/range.hpp"

namespace ltb::aero::details
{

template <>
auto Airspeed< AircraftType::RobinsonR22BetaII >::look_up_cas_knots(
    CasParameters< float64 > const& parameters
) -> float64
{
    // The R22 POH provides a table of indicated airspeeds and their corresponding
    // calibrated airspeeds. The table appears to be linearly interpolated between
    // the indicated airspeeds and their corresponding calibrated airspeeds.
    constexpr auto indicated_kts_range  = math::Range{ .min = 27.5, .max = 102.0 };
    constexpr auto calibrated_kts_range = math::Range{ .min = 29.5, .max = 97.5 };

    return to_new_range(
        parameters.indicated_airspeed_knots,
        indicated_kts_range,
        calibrated_kts_range
    );
}

template <>
auto Airspeed< AircraftType::RobinsonR44II >::look_up_cas_knots(
    CasParameters< float64 > const& parameters
) -> float64
{
    // The R44 II POH provides a table of indicated airspeeds and their corresponding
    // calibrated airspeeds. The table appears to be linearly interpolated between
    // the indicated airspeeds and their corresponding calibrated airspeeds.
    constexpr auto indicated_kts_range  = math::Range{ .min = 20.0, .max = 140.0 };
    constexpr auto calibrated_kts_range = math::Range{ .min = 21.0, .max = 136.0 };

    return to_new_range(
        parameters.indicated_airspeed_knots,
        indicated_kts_range,
        calibrated_kts_range
    );
}

} // namespace ltb::aero::details
