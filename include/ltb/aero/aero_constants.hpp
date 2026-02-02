#pragma once

// standard
#include <type_traits>

namespace ltb::aero
{

template < typename T >
    requires std::is_floating_point_v< T >
struct Constants
{
    // (J/mol·K)
    static constexpr auto molar_gas_constant = T( 8.31446261815324 );
    // (kg/mol)
    static constexpr auto molar_mass_dry_air = T( 0.02896968 );
    // (J/kg·K)
    static constexpr auto dry_air_gas_constant = molar_gas_constant / molar_mass_dry_air;
    // At 15°C
    static constexpr auto dry_air_heat_capacity_ratio = T( 1.4 );

    // Unitless constant found in airspeed equations
    static constexpr auto heat_capacity_constant
        = ( dry_air_heat_capacity_ratio - T( 1 ) ) / dry_air_heat_capacity_ratio;
    // Unitless constant found in airspeed equations
    static constexpr auto inv_heat_capacity_constant
        = dry_air_heat_capacity_ratio / ( dry_air_heat_capacity_ratio - T( 1 ) );

    // (m/s^2)
    static constexpr auto gravity = T( 9.80665 );

    // (Pa)
    static constexpr auto standard_pressure = T( 101'325.0 );
    // (K)
    static constexpr auto standard_temperature = T( 288.15 );
    // (K/m) Lapse rate below 11 km (36,090 ft)
    static constexpr auto standard_temp_lapse_rate = T( 0.0065 );
    // Kg/m^3
    static constexpr auto standard_density = T( 1.225 );

    // Unitless exponent found in many altitude equations
    static constexpr auto common_exponent
        = standard_temp_lapse_rate * ( dry_air_gas_constant / gravity );
    // Unitless exponent found in many altitude equations
    static constexpr auto inv_common_exponent
        = gravity / ( dry_air_gas_constant * standard_temp_lapse_rate );

    // m
    static constexpr auto standard_lapse_dist = standard_temperature / standard_temp_lapse_rate;
    // 1/m
    static constexpr auto inv_standard_lapse_dist = standard_temp_lapse_rate / standard_temperature;
};

} // namespace ltb::aero
