#pragma once

// project
#include "ltb/aero/aero_constants.hpp"
#include "ltb/aero/aircraft_type.hpp"
#include "ltb/utils/units.hpp"

// standard
#include <cmath>

namespace ltb::aero
{

/// \brief Compute the indicated airspeed from the static air pressure
///        and the pitot tube differential pressure.
/// \param differential_pressure_pa The difference between total pitot
///        tube pressure and static pressure in Pascals.
/// \param static_pressure_pa The static pressure of the air in Pascals.
/// \returns The indicated airspeed in meters per second.
template < typename T >
    requires std::is_floating_point_v< T >
auto indicated_airspeed( T const differential_pressure_pa, T const static_pressure_pa ) -> T
{
    static_assert( std::is_floating_point_v< T >, "T must be a floating point type." );

    auto const total_pressure  = static_pressure_pa + differential_pressure_pa;
    auto const pressure_ratio  = total_pressure / static_pressure_pa;
    auto const pressure_volume = ( static_pressure_pa / Constants< T >::standard_density );

    auto const coeff = Constants< T >::inv_heat_capacity_constant * T( 2 ) * pressure_volume;
    auto const exp   = Constants< T >::heat_capacity_constant;

    return std::sqrt( coeff * ( std::pow( pressure_ratio, exp ) - T( 1 ) ) );
}

/// \brief Compute the indicated airspeed from the static air pressure
///        and the pitot tube differential pressure.
/// \param differential_pressure_pa The difference between total pitot
///        tube pressure and static pressure in Pascals.
/// \param static_pressure_pa The static pressure of the air in Pascals.
/// \returns The indicated airspeed in knots.
template < typename T >
    requires std::is_floating_point_v< T >
auto indicated_airspeed_knots( T const differential_pressure_pa, T const static_pressure_pa ) -> T
{
    auto const ias_m_per_s = indicated_airspeed( differential_pressure_pa, static_pressure_pa );
    return utils::kts_from_m_per_s( ias_m_per_s );
}

/// \brief Parameters for the Calibrated Airspeed (CAS) calculation.
template < typename T >
struct CasParameters
{
    T indicated_airspeed_knots = T( 0 );
    // Can add more parameters if aircraft have pitot heat or
    // floats installed since those can affect the CAS.
};

/// \brief Returns the Calibrated Airspeed (CAS) value in knots for
///        the provided Indicated Airspeed (IAS). Aircraft without
///        a calibrated airspeed table will return the IAS value.
/// \param aircraft The aircraft class.
/// \param params The CAS parameters.
/// \returns The CAS value in knots.
template < typename T >
auto look_up_cas_knots( AircraftType aircraft, CasParameters< T > const& params ) -> T;

/// \brief The speed of sound in dry air at a given temperature.
/// \param temperature_k The temperature in Kelvin.
/// \returns The speed of sound in meters per second.
template < typename T >
auto speed_of_sound( T const temperature_k ) -> T
{
    return std::sqrt(
        Constants< T >::dry_air_heat_capacity_ratio * Constants< T >::dry_air_gas_constant
        * temperature_k
    );
}

/// \brief The dynamic pressure at sea level for a given calibrated airspeed.
/// \param cas_m_per_s The calibrated airspeed in meters per second.
/// \returns The dynamic pressure in Pascals.
template < typename T >
auto sea_level_dynamic_pressure( T const cas_m_per_s ) -> T
{
    auto const scale = ( Constants< T >::dry_air_heat_capacity_ratio - T( 1 ) ) / T( 2 );

    auto const speed_ratio = cas_m_per_s / speed_of_sound( Constants< T >::standard_temperature );
    auto const base        = ( ( speed_ratio * speed_ratio ) * scale ) + T( 1 );
    auto const exp         = Constants< T >::inv_heat_capacity_constant;

    return Constants< T >::standard_pressure * ( std::pow( base, exp ) - T( 1 ) );
}

/// \brief The Mach number for a given calibrated airspeed and static pressure.
/// \param cas_m_per_s The calibrated airspeed in meters per second.
/// \param static_pressure_pa The static pressure in Pascals.
/// \returns The Mach number.
template < typename T >
auto mach_number( T const cas_m_per_s, T const static_pressure_pa ) -> T
{
    auto const dynamic_pressure_pa = sea_level_dynamic_pressure( cas_m_per_s );

    auto const coeff = T( 2 ) / ( Constants< T >::dry_air_heat_capacity_ratio - T( 1 ) );
    auto const base  = ( dynamic_pressure_pa / static_pressure_pa ) + T( 1 );
    auto const exp   = Constants< T >::heat_capacity_constant;

    return std::sqrt( coeff * ( std::pow( base, exp ) - T( 1 ) ) );
}

/// \brief Parameters for the True Airspeed (TAS) calculation.
template < typename T >
struct TasParams
{
    T cas_m_per_s        = T( 0 );
    T static_pressure_pa = Constants< T >::standard_pressure;
    T temperature_k      = Constants< T >::standard_temperature;
};

/// \brief Returns the True Airspeed (TAS) value in meters per second
/// \details https://aviation.stackexchange.com/a/77300/72026
template < typename T >
auto true_airspeed( TasParams< T > const& params ) -> T
{
    return mach_number( params.cas_m_per_s, params.static_pressure_pa )
         * speed_of_sound( params.temperature_k );
}

/// \brief Parameters for the True Airspeed (TAS) calculation.
template < typename T >
struct TasKnotsParams
{
    T cas_knots          = T( 0 );
    T static_pressure_pa = T( 0 );
    T temperature_c      = T( 15 );
};

/// \brief Returns the True Airspeed (TAS) value in knots.
template < typename T >
auto true_airspeed_knots( TasKnotsParams< T > const& params ) -> T
{
    auto converted               = TasParams< T >{ };
    converted.cas_m_per_s        = utils::m_per_s_from_kts< T >( params.cas_knots );
    converted.static_pressure_pa = params.static_pressure_pa;
    converted.temperature_k      = utils::k_from_c( params.temperature_c );

    auto const tas_m_per_s = true_airspeed( converted );

    return utils::kts_from_m_per_s( tas_m_per_s );
}

} // namespace ltb::aero
