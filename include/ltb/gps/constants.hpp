#pragma once

namespace ltb::gps
{

template < typename T >
class Constants
{
public:
    static constexpr auto satellite_altitude_Mm( ) { return T( 20.2 ); }

    static constexpr auto earth_radius_Mm( ) { return T( 6.378 ); }

    static constexpr auto speed_of_light_m_ns( ) { return T( 0.299792458 ); }

    static constexpr auto Mm_from_ft( T const ft ) { return ft * T( 3.048e-7 ); }
};

} // namespace ltb::gps
