
// project
#include "ltb/aero/altitude.hpp"
#include "ltb/utils/ignore.hpp"
#include "ltb/utils/types.hpp"

// external
#include <gtest/gtest.h>

// standard
#include <algorithm>
#include <random>

namespace ltb
{
namespace
{

// Less than 2 cm seems like a reasonable error margin.
template < typename T >
constexpr auto error_epsilon( ) -> T;

template <>
constexpr auto error_epsilon< float32 >( ) -> float32
{
    return 0.02F;
}

template <>
constexpr auto error_epsilon< float64 >( ) -> float64
{
    return 0.0001;
}

template < typename T >
constexpr auto sea_level_pressure_pa( )
{
    return T{ 101'325 };
}

template < typename T >
struct AltParams
{
    T altitude_m;
    T pressure_pa;
};

template < typename T >
class AltitudeTests : public ::testing::Test
{
public:
    static auto station_alt_setting( AltParams< T > const& station ) -> T
    {
        // This is the standard altitude an altimeter would show without
        // a correction based on the pressure at the current station.
        auto const standard_altitude_m = aero::standard_altitude_at_pressure( station.pressure_pa );

        // This is the difference between that standard altitude
        // and the actual altitude at the weather station.
        auto const altitude_offset = standard_altitude_m - station.altitude_m;

        // This is the pressure offset that should be applied to altimeters to
        // display the correct altitude when near the station.
        return aero::standard_pressure_at_altitude( altitude_offset );
    }
};

using FloatTypes = ::testing::Types< float32, float64 >;
TYPED_TEST_SUITE( AltitudeTests, FloatTypes, );

TEST( AltitudeTests, StandardAltitude )
{
    EXPECT_FLOAT_EQ(
        aero::indicated_altitude_m( sea_level_pressure_pa< float32 >( ), 101'325.0F ),
        0.0F
    );
    EXPECT_FLOAT_EQ(
        aero::standard_altitude_at_pressure( sea_level_pressure_pa< float32 >( ) ),
        0.0F
    );
    EXPECT_FLOAT_EQ(
        aero::standard_pressure_at_altitude( 0.0F ),
        sea_level_pressure_pa< float32 >( )
    );

    EXPECT_DOUBLE_EQ(
        aero::indicated_altitude_m( sea_level_pressure_pa< float64 >( ), 101'325.0 ),
        0.0
    );
    EXPECT_DOUBLE_EQ(
        aero::standard_altitude_at_pressure( sea_level_pressure_pa< float64 >( ) ),
        0.0
    );
    EXPECT_DOUBLE_EQ(
        aero::standard_pressure_at_altitude( 0.0 ),
        sea_level_pressure_pa< float64 >( )
    );
}

TYPED_TEST( AltitudeTests, IndicatedAltitude100m )
{
    using T = TypeParam;

    auto params = AltParams< T >{
        .altitude_m  = T{ 100 },
        .pressure_pa = sea_level_pressure_pa< T >( ),
    };
    auto alt_setting_pa = AltitudeTests< T >::station_alt_setting( params );
    EXPECT_NEAR(
        aero::indicated_altitude_m( params.pressure_pa, alt_setting_pa ),
        params.altitude_m,
        error_epsilon< T >( )
    );

    params.pressure_pa = T{ 101'000 };
    alt_setting_pa     = AltitudeTests< T >::station_alt_setting( params );
    EXPECT_NEAR(
        aero::indicated_altitude_m( params.pressure_pa, alt_setting_pa ),
        params.altitude_m,
        error_epsilon< T >( )
    );

    params.pressure_pa = T{ 102'000 };
    alt_setting_pa     = AltitudeTests< T >::station_alt_setting( params );
    EXPECT_NEAR(
        aero::indicated_altitude_m( params.pressure_pa, alt_setting_pa ),
        params.altitude_m,
        error_epsilon< T >( )
    );

    params.pressure_pa = T{ 100'000 };
    alt_setting_pa     = AltitudeTests< T >::station_alt_setting( params );
    EXPECT_NEAR(
        aero::indicated_altitude_m( params.pressure_pa, alt_setting_pa ),
        params.altitude_m,
        error_epsilon< T >( )
    );
}

TYPED_TEST( AltitudeTests, IndicatedAltitude500m )
{
    using T = TypeParam;

    auto params = AltParams< T >{
        .altitude_m  = T{ 500 },
        .pressure_pa = sea_level_pressure_pa< T >( ),
    };
    auto alt_setting_pa = AltitudeTests< T >::station_alt_setting( params );
    EXPECT_NEAR(
        aero::indicated_altitude_m( params.pressure_pa, alt_setting_pa ),
        params.altitude_m,
        error_epsilon< T >( )
    );

    params.pressure_pa = T{ 101'094 };
    alt_setting_pa     = AltitudeTests< T >::station_alt_setting( params );
    EXPECT_NEAR(
        aero::indicated_altitude_m( params.pressure_pa, alt_setting_pa ),
        params.altitude_m,
        error_epsilon< T >( )
    );

    params.pressure_pa = T{ 102'987 };
    alt_setting_pa     = AltitudeTests< T >::station_alt_setting( params );
    EXPECT_NEAR(
        aero::indicated_altitude_m( params.pressure_pa, alt_setting_pa ),
        params.altitude_m,
        error_epsilon< T >( )
    );

    params.pressure_pa = T{ 100'123 };
    alt_setting_pa     = AltitudeTests< T >::station_alt_setting( params );
    EXPECT_NEAR(
        aero::indicated_altitude_m( params.pressure_pa, alt_setting_pa ),
        params.altitude_m,
        error_epsilon< T >( )
    );
}

TYPED_TEST( AltitudeTests, RandomValues )
{
    using T = TypeParam;

    struct RandomParams
    {
        int32 const  seed = ::testing::UnitTest::GetInstance( )->random_seed( );
        std::mt19937 gen  = std::mt19937{ static_cast< std::mt19937::result_type >( seed ) };
        std::uniform_real_distribution< T > alt_dist
            = std::uniform_real_distribution< T >{ T{ -100 }, T{ +10'000 } };
        std::uniform_real_distribution< T > pres_dist
            = std::uniform_real_distribution< T >{ T{ 5'000 }, T{ +103'000 } };

        RandomParams( )
        {
            std::cout << "[          ] Random seed: " << seed << std::endl;
        }

        auto operator( )( )
        {
            return AltParams< T >{
                .altitude_m  = alt_dist( gen ),
                .pressure_pa = pres_dist( gen ),
            };
        }
    };

    constexpr auto value_count = 50'000UL;

    auto params = std::vector< AltParams< T > >( value_count );
    utils::ignore( std::ranges::generate( params, RandomParams{ } ) );

    for ( auto const& p : params )
    {
        auto const alt_setting_pa = AltitudeTests< T >::station_alt_setting( p );
        EXPECT_NEAR(
            aero::indicated_altitude_m( p.pressure_pa, alt_setting_pa ),
            p.altitude_m,
            error_epsilon< T >( )
        ) << "Altitude: "
          << p.altitude_m << " Pressure: " << p.pressure_pa;
    }
}

// Tests that failed from the random testing above can be reproduced here.
TYPED_TEST( AltitudeTests, FailedCases )
{
    using T = TypeParam;

    auto params = AltParams< T >{
        .altitude_m  = T{ 8'015.80712890625 },
        .pressure_pa = T{ 101'334.9375 },
    };
    auto alt_setting_pa = AltitudeTests< T >::station_alt_setting( params );
    EXPECT_NEAR(
        aero::indicated_altitude_m( params.pressure_pa, alt_setting_pa ),
        params.altitude_m,
        error_epsilon< T >( )
    );
}

} // namespace
} // namespace ltb
