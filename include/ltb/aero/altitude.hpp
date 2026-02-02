#pragma once

// project
#include "ltb/aero/aero_constants.hpp"

// standard
#include <cmath>
#include <concepts>

// Most of these calculations are more accurate versions of the
// formulas found on weather.gov:
// https://www.weather.gov/media/epz/wxcalc/altimeterSetting.pdf
// https://www.weather.gov/media/epz/wxcalc/pressureAltitude.pdf
//
// Some wonderful humans worked out the relationship to the barometric
// formula and the various derivations of the altitude equations:
// https://aviation.stackexchange.com/a/91250/72026
//
// The random 0.3 correction thrown into some of the equations comes
// from the 1951 Smithsonian Meteorological Tables which state:
//
//    The correction to ρ of -0.3 mb. (or -0.01 in. Hg.) arises from the
//    fact that the altimeter of an airplane is usually about 10 feet
//    above the landing gear and it is desirable to indicate the height
//    of the landing gear rather than the height of the cockpit.
//
// We do not use this 0.3 correction in our calculations.
//
// Further work to appropriately rearrange the above equations can be found here:
// https://logan-barnes.notion.site/Altimeter-Setting-15eb173d1f4f80d7b459dc6b50ccf03b

namespace ltb::aero
{

/// \brief Compute the indicated altitude from the static air pressure and an altimeter setting.
///        Using an altimeter setting of 101325 Pa (~29.92 inHg) will yield the pressure altitude.
/// \param static_pressure_pa The static pressure of the air in Pascals.
/// \param altimeter_setting_pa The altimeter setting in Pascals.
/// \returns The indicated altitude in meters.
template < typename T >
    requires std::is_floating_point_v< T >
auto indicated_altitude_m( T const static_pressure_pa, T const altimeter_setting_pa ) -> T
{
    static_assert( std::is_floating_point_v< T >, "T must be a floating point type." );

    auto const altimeter_setting_rate
        = std::pow( altimeter_setting_pa, Constants< T >::common_exponent );
    auto const static_pressure_rate
        = std::pow( static_pressure_pa, Constants< T >::common_exponent );
    auto const rate_diff = altimeter_setting_rate - static_pressure_rate;

    /// \code
    /// auto const standard_pressure_rate = std::pow(Constants<T>::standard_pressure,
    /// Constants<T>::common_exponent); auto const coeff = Constants<T>::standard_lapse_dist /
    /// standard_pressure_rate;
    /// \endcode
    // The original code above was precomputed to be:
    constexpr auto coeff = T( 4948.330223950178 );

    return coeff * rate_diff;
}

/// \brief Compute the altitude in the standard atmosphere at a given pressure.
/// \param pressure_pa The pressure in Pascals.
/// \returns The altitude in meters.
template < typename T >
    requires std::is_floating_point_v< T >
auto standard_altitude_at_pressure( T pressure_pa ) -> T
{
    constexpr auto exp   = Constants< T >::common_exponent;
    auto const     base  = pressure_pa / Constants< T >::standard_pressure;
    constexpr auto coeff = Constants< T >::standard_lapse_dist;
    return coeff * ( T( 1 ) - std::pow( base, exp ) );
}

/// \brief Compute the air pressure in the standard atmosphere at a given altitude.
/// \param altitude_m The altitude in meters.
/// \returns The pressure in Pascals.
template < typename T >
    requires std::is_floating_point_v< T >
auto standard_pressure_at_altitude( T const altitude_m ) -> T
{
    constexpr auto exp   = Constants< T >::inv_common_exponent;
    auto const     base  = T( 1 ) - ( Constants< T >::inv_standard_lapse_dist * altitude_m );
    constexpr auto coeff = Constants< T >::standard_pressure;
    return coeff * std::pow( base, exp );
}

} // namespace ltb::aero
